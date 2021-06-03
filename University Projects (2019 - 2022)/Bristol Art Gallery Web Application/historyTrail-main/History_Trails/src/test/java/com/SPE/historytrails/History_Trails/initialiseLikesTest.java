package com.SPE.historytrails.History_Trails;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;

import static org.assertj.core.api.Assertions.assertThat;

@SpringBootTest
public class initialiseLikesTest {

    @Test
    void testNumberOfObjects(){
        JSONArray likesList = HistoryTrailsApplication.initialiseLikes();
        assertThat(likesList.size() == 115);

    }

    @Test
    void testRecordIdField(){
        JSONArray likesList = HistoryTrailsApplication.initialiseLikes();
        for (Object object: likesList) {
            JSONObject objects = (JSONObject) object;
            assertThat(objects.get("recordid") != null);
        }
    }

    @Test
    void testLikesIdField(){
        JSONArray likesList = HistoryTrailsApplication.initialiseLikes();
        for (Object object: likesList) {
            JSONObject objects = (JSONObject) object;
            assertThat(objects.get("likes") != null);
        }
    }

    @Test
    void countFieldsEqualsTwo(){
        JSONArray likesList = HistoryTrailsApplication.initialiseLikes();
        for (Object object: likesList) {
            JSONObject objects = (JSONObject) object;
            assertThat(objects.size() == 2);
        }
    }


    @Test
    void testLikesEqualZero(){
        JSONArray likesList = HistoryTrailsApplication.initialiseLikes();
        for (Object object: likesList) {
            JSONObject objects = (JSONObject) object;
            assertThat(objects.get("likes").equals(0));
        }

    }
}
