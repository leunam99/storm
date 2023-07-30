#include "test/storm_gtest.h"
#include "storm/modelchecker/blackbox/storage/HashStorage.h"
#include <utility> 


TEST(HashStorage, stateExists) {
       auto hashStorage = storm::modelchecker::blackbox::storage::HashStorage<uint_fast64_t>();
       hashStorage.addState(1);
       ASSERT_TRUE(hashStorage.stateExists(1)); 
}

TEST(HashStorage, incTransNewState) {
        auto hashStorage = storm::modelchecker::blackbox::storage::HashStorage<uint_fast64_t>();
        hashStorage.incTrans(1,2,3,10);
        ASSERT_TRUE(hashStorage.stateExists(1));
        ASSERT_EQ(hashStorage.getSuccSamples(1,2,3), 10);
}

TEST(HashStorage, incTransExistingState) {
        auto hashStorage = storm::modelchecker::blackbox::storage::HashStorage<uint_fast64_t>();
        hashStorage.addState(1);
        hashStorage.incTrans(1,2,3,10);
        ASSERT_TRUE(hashStorage.stateExists(1));
        ASSERT_EQ(hashStorage.getSuccSamples(1,2,3), 10);
        ASSERT_EQ(hashStorage.getTotalStateCount(), 2);
}

TEST(HashStorage, getTotalSamplesForState) {
        auto hashStorage = storm::modelchecker::blackbox::storage::HashStorage<uint_fast64_t>();
        hashStorage.incTrans(1,2,3,10);
        hashStorage.incTrans(1,2,4,11);
        hashStorage.incTrans(1,2,5,9);

        ASSERT_EQ(hashStorage.getTotalSamples(1,2), 30);
}

TEST(HashStorage, reverseMap) {
        auto hashStorage = storm::modelchecker::blackbox::storage::HashStorage<uint_fast64_t>();
        hashStorage.incTrans(3,10,1,1);
        hashStorage.incTrans(2,10,1,1);
        hashStorage.incTrans(2,11,1,1); //Add a transition with different action

        hashStorage.createReverseMapping();
        ASSERT_EQ(hashStorage.getPredecessors(1).size(), 3);
}


TEST(HashStorage, logTotals) {
        auto hashStorage = storm::modelchecker::blackbox::storage::HashStorage<uint_fast64_t>();
        
        hashStorage.incTrans(1,30,2,1);
        hashStorage.incTrans(1,30,3,1);
        hashStorage.incTrans(1,30,4,1);
        hashStorage.incTrans(1,30,5,1);
        hashStorage.incTrans(1,30,5,10); //Incrementing transition without adding new states 

        //Transition from state that was added beforehand 
        hashStorage.incTrans(2,30,1,10); 
        //Unsampled action 
        hashStorage.addUnsampledAction(1,40); 

        ASSERT_EQ(hashStorage.getTotalStateCount(), 5);
        ASSERT_EQ(hashStorage.getTotalTransitionCount(), 5);
        ASSERT_EQ(hashStorage.gettotalStateActionPairCount(), 3);
}


