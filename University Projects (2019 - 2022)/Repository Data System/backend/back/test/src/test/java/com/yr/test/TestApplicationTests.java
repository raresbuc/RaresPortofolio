package com.yr.test;

import demo.RepoDemo;
import demo.RepoLang;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpResponseException;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;
import org.springframework.boot.test.context.SpringBootTest;
import utils.R;

import java.io.IOException;

@SpringBootTest

class TestApplicationTests {

    private String test_url = "https://github.com/spe-uob/";
    private String test_token = "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";


    @Test
    void getProj() throws IOException {
        RepoDemo test = new RepoDemo();
        assertThrows(IOException.class,() -> test.getProj(test_url,""));
        Exception e = assertThrows(IOException.class, () -> test.getProj("/spe-uob/",test_token));
        assertEquals("status code: 404, reason phrase: Not Found", e.getMessage());
        assertNotEquals(test_url, test_token);
    }


    @Test
    void getAllUser() {
        RepoDemo test = new RepoDemo();
        assertThrows(IOException.class,() -> test.getUser(test_url,""));
        Exception e = assertThrows(IOException.class, () -> test.getProj("/spe-uob/",test_token));
        assertEquals("status code: 404, reason phrase: Not Found", e.getMessage());
    }
}
