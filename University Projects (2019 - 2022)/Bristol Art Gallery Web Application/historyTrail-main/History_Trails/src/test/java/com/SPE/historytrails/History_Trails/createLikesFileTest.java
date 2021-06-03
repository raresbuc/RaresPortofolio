package com.SPE.historytrails.History_Trails;

import org.json.simple.JSONArray;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;
import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import static org.assertj.core.api.Assertions.assertThat;

@SpringBootTest
public class createLikesFileTest {


    @Test
    public void writesContentToFileTest() throws IOException {

        JSONArray likesList = HistoryTrailsApplication.initialiseLikes();
        HistoryTrailsApplication.createLikesfile();

        JSONParser jsonParser = new JSONParser();

        try (FileReader reader = new FileReader("src/main/resources/json/likes.json")) {
            Object obj = jsonParser.parse(reader);
            JSONArray objectFileList = (JSONArray) obj;

            assertThat(objectFileList.toJSONString().equals(likesList.toJSONString()));

        } catch (FileNotFoundException | ParseException e) {
            e.printStackTrace();
        }
    }
}

