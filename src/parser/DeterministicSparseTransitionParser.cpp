/*!
 *	TraParser.cpp
 *
 *	Created on: 20.11.2012
 *		Author: Gereon Kremer
 */

#include "src/parser/DeterministicSparseTransitionParser.h"

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <clocale>
#include <iostream>
#include <string>

#include "src/utility/constants.h"
#include "src/utility/cstring.h"
#include "src/parser/MappedFile.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/settings/Settings.h"

#include "ReadValues.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
	namespace parser {

		using namespace storm::utility::cstring;

        template<typename T>
		storm::storage::SparseMatrix<T> DeterministicSparseTransitionParser::parseDeterministicTransitions(std::string const& filename) {

			storm::storage::SparseMatrix<T> emptyMatrix;            
			return DeterministicSparseTransitionParser::parse(filename, false, emptyMatrix);
            
		}

        template<typename T>
		storm::storage::SparseMatrix<T> DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(std::string const& filename, storm::storage::SparseMatrix<T> const & transitionMatrix) {

			return DeterministicSparseTransitionParser::parse(filename, true, transitionMatrix);
		}

        template<typename T>
		storm::storage::SparseMatrix<T> DeterministicSparseTransitionParser::parse(std::string const& filename, bool isRewardFile, storm::storage::SparseMatrix<T> const & transitionMatrix) {
			// Enforce locale where decimal point is '.'.
				setlocale(LC_NUMERIC, "C");

				if (!MappedFile::fileExistsAndIsReadable(filename.c_str())) {
					LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
					throw storm::exceptions::FileIoException() << "The supplied Transition input file \"" << filename << "\" does not exist or is not readable by this process.";
				}

				// Open file.
				MappedFile file(filename.c_str());
				char const* buf = file.getData();

				// Perform first pass, i.e. count entries that are not zero.
				bool insertDiagonalEntriesIfMissing = !isRewardFile;
				DeterministicSparseTransitionParser::FirstPassResult firstPass = DeterministicSparseTransitionParser::firstPass(file.getData(), insertDiagonalEntriesIfMissing);

				LOG4CPLUS_INFO(logger, "First pass on " << filename << " shows " << firstPass.numberOfNonzeroEntries << " NonZeros.");

				// If first pass returned zero, the file format was wrong.
				if (firstPass.numberOfNonzeroEntries == 0) {
					LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": empty or erroneous file format.");
					throw storm::exceptions::WrongFormatException();
				}

                
				if(isRewardFile) {
					// The reward matrix should match the size of the transition matrix.
					if (firstPass.highestStateIndex + 1 > transitionMatrix.getRowCount() || firstPass.highestStateIndex + 1 > transitionMatrix.getColumnCount()) {
						LOG4CPLUS_ERROR(logger, "Reward matrix has more rows or columns than transition matrix.");
						throw storm::exceptions::WrongFormatException() << "Reward matrix has more rows or columns than transition matrix.";
					} else {
						// If we found the right number of states or less, we set it to the number of states represented by the transition matrix.
						firstPass.highestStateIndex = transitionMatrix.getRowCount() - 1;
					}
				}
				// Perform second pass.

				// Skip the format hint if it is there.
				buf = skipFormatHint(buf);


				// Creating matrix builder here.
				// The actual matrix will be build once all contents are inserted.
				storm::storage::SparseMatrixBuilder<T> resultMatrix(firstPass.highestStateIndex + 1, firstPass.highestStateIndex + 1, firstPass.numberOfNonzeroEntries);

				uint_fast64_t lastRow = 0;
				DeterministicTransitionEntry<T> trans;
				bool fixDeadlocks = storm::settings::Settings::getInstance()->isSet("fixDeadlocks");
				bool hadDeadlocks = false;
				bool rowHadDiagonalEntry = false;


				// Read all transitions from file. Note that we assume that the
				// transitions are listed in canonical order, otherwise this will not
				// work, i.e. the values in the matrix will be at wrong places.

				// Different parsing routines for transition systems and transition rewards.
				if(isRewardFile) {
					while (buf[0] != '\0') {
						// Read next transition.
                        readNextTransition(&buf, &trans);
						addTransitionToMatrix(trans, &resultMatrix);
					}
				} else {
                    // Read first row and add self-loops if necessary.
                    
                    readNextTransition(&buf, &trans);
                    
                    if (trans.row > 0) {
                        for (uint_fast64_t skippedRow = 0; skippedRow < trans.row; ++skippedRow) {
                            hadDeadlocks = true;
                            if (fixDeadlocks) {
                                resultMatrix.addNextValue(skippedRow, skippedRow, storm::utility::constantOne<T>());
                                LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << skippedRow << " has no outgoing transitions. A self-loop was inserted.");
                            } else {
                                LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": state " << skippedRow << " has no outgoing transitions.");
                                // Before throwing the appropriate exception we will give notice of all deadlock states.
                            }
                        }
                    }
                    addTransitionToMatrix(trans, &resultMatrix);
					
                    
					while (buf[0] != '\0') {

						// Read next transition.
						readNextTransition(&buf, &trans);

						// Test if we moved to a new row.
						// Handle all incomplete or skipped rows.
						if (lastRow != trans.row) {
							if (!rowHadDiagonalEntry) {
								if (insertDiagonalEntriesIfMissing) {
									resultMatrix.addNextValue(lastRow, lastRow, storm::utility::constantZero<T>());
									LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << lastRow << " has no transition to itself. Inserted a 0-transition. (1)");
								} else {
									LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << lastRow << " has no transition to itself.");
								}
								// No increment for lastRow.
								rowHadDiagonalEntry = true;
							}
							for (uint_fast64_t skippedRow = lastRow + 1; skippedRow < trans.row; ++skippedRow) {
								hadDeadlocks = true;
								if (fixDeadlocks) {
									resultMatrix.addNextValue(skippedRow, skippedRow, storm::utility::constantOne<T>());
									LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << skippedRow << " has no outgoing transitions. A self-loop was inserted.");
								} else {
									LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": state " << skippedRow << " has no outgoing transitions.");
									// Before throwing the appropriate exception we will give notice of all deadlock states.
								}
							}
							lastRow = trans.row;
							rowHadDiagonalEntry = false;
						}

						if (trans.col == trans.row) {
							rowHadDiagonalEntry = true;
						}

						if (trans.col > trans.row && !rowHadDiagonalEntry) {
							if (insertDiagonalEntriesIfMissing) {
								resultMatrix.addNextValue(trans.row, trans.row, storm::utility::constantZero<T>());
								LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << trans.row << " has no transition to itself. Inserted a 0-transition. (2)");
							} else {
								LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << trans.row << " has no transition to itself.");
							}
							rowHadDiagonalEntry = true;
						}

						addTransitionToMatrix(trans, &resultMatrix);
					}

					if (!rowHadDiagonalEntry) {
						if (insertDiagonalEntriesIfMissing) {
							resultMatrix.addNextValue(lastRow, lastRow, storm::utility::constantZero<T>());
							LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << lastRow << " has no transition to itself. Inserted a 0-transition. (3)");
						} else {
							LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << lastRow << " has no transition to itself.");
						}
					}

                    // If we encountered deadlock and did not fix them, now is the time to throw the exception.
                	if (!fixDeadlocks && hadDeadlocks) throw storm::exceptions::WrongFormatException() << "Some of the nodes had deadlocks. You can use --fixDeadlocks to insert self-loops on the fly.";
				}

				// Finally, build the actual matrix, test and return it.
				storm::storage::SparseMatrix<T> result = resultMatrix.build();

				// Since we cannot do the testing if each transition for which there is a reward in the reward file also exists in the transition matrix during parsing, we have to do it afterwards.
				if(isRewardFile && !result.isSubmatrixOf(transitionMatrix)) {
					LOG4CPLUS_ERROR(logger, "There are rewards for non existent transitions given in the reward file.");
					throw storm::exceptions::WrongFormatException() << "There are rewards for non existent transitions given in the reward file.";
				}

				return result;
		}

		DeterministicSparseTransitionParser::FirstPassResult DeterministicSparseTransitionParser::firstPass(char const* buf, bool insertDiagonalEntriesIfMissing) {

			DeterministicSparseTransitionParser::FirstPassResult result;

			// Skip the format hint if it is there.
			buf = skipFormatHint(buf);

			 // Check all transitions for non-zero diagonal entries and deadlock states.
			uint_fast64_t row, col, lastRow = 0, lastCol = -1;
			bool rowHadDiagonalEntry = false;
            
            // Read first row and reserve space for self-loops if necessary.
            char const* tmp;
            row = checked_strtol(buf, &tmp);
            if (row > 0) {
                for (uint_fast64_t skippedRow = 0; skippedRow < row; ++skippedRow) {
                    ++result.numberOfNonzeroEntries;
                }
            }
            
			while (buf[0] != '\0') {

				// Read the transition.
				row = checked_strtol(buf, &buf);
				col = checked_strtol(buf, &buf);
				// The actual read value is not needed here.
				buf = forwardToLineEnd(buf);
                buf = trimWhitespaces(buf);

				// Compensate for missing diagonal entries if desired.
				if (insertDiagonalEntriesIfMissing) {
					if (lastRow != row) {
						if(!rowHadDiagonalEntry) {
							++result.numberOfNonzeroEntries;
						}

						// Compensate for missing rows.
						for (uint_fast64_t skippedRow = lastRow + 1; skippedRow < row; ++skippedRow) {
							++result.numberOfNonzeroEntries;
						}
						rowHadDiagonalEntry = false;
					}

					if (col == row) {
						rowHadDiagonalEntry = true;
					}

					if (col > row && !rowHadDiagonalEntry) {
						rowHadDiagonalEntry = true;
						++result.numberOfNonzeroEntries;
					}
				}

				// Check if a higher state id was found.
				if (row > result.highestStateIndex) result.highestStateIndex = row;
				if (col > result.highestStateIndex) result.highestStateIndex = col;

				++result.numberOfNonzeroEntries;

				// Have we already seen this transition?
				if (row == lastRow && col == lastCol) {
					LOG4CPLUS_ERROR(logger, "The same transition (" << row << ", " <<  col << ") is given twice.");
					throw storm::exceptions::InvalidArgumentException() << "The same transition (" << row << ", " << col << ") is given twice.";
				}

				lastRow = row;
				lastCol = col;

				
			}

			if(insertDiagonalEntriesIfMissing) {
				if (!rowHadDiagonalEntry) {
					++result.numberOfNonzeroEntries;
				}

				//Compensate for missing rows at the end of the file.
				for (uint_fast64_t skippedRow = (uint_fast64_t)(lastRow + 1); skippedRow <= result.highestStateIndex; ++skippedRow) {
					++result.numberOfNonzeroEntries;
				}
			}

			return result;
		}
        
        template<typename T>
        void DeterministicSparseTransitionParser::readNextTransition(char const** buf, DeterministicTransitionEntry<T>* trans)
        {
            trans->row = checked_strtol(*buf, buf);
            trans->col = checked_strtol(*buf, buf);
            trans->val = checked_strtod(*buf, buf);
            
            *buf = trimWhitespaces(*buf);
        }
        
        
        
        template<typename T>
        void DeterministicSparseTransitionParser::addTransitionToMatrix(DeterministicTransitionEntry<T> const& trans, storm::storage::SparseMatrixBuilder<T>* mat)
        {
            mat->addNextValue(trans.row, trans.col, trans.val);
        }

        char const* DeterministicSparseTransitionParser::skipFormatHint(char const* buf)
        {
            // Skip the format hint if it is there.
			buf = trimWhitespaces(buf);
			if(buf[0] < '0' || buf[0] > '9') {
				buf = forwardToLineEnd(buf);
				buf = trimWhitespaces(buf);
			}
            return buf;
        }

        template storm::storage::SparseMatrix<double> DeterministicSparseTransitionParser::parseDeterministicTransitions<double>(std::string const& filename);
        template storm::storage::SparseMatrix<double> DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(std::string const& filename, storm::storage::SparseMatrix<double> const & transitionMatrix);

	}  // namespace parser
}  // namespace storm
