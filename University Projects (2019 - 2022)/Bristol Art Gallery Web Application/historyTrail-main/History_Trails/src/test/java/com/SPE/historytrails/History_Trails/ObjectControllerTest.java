package com.SPE.historytrails.History_Trails;

import com.SPE.historytrails.History_Trails.controller.ObjectController;
import com.SPE.historytrails.History_Trails.domain.Objects;
import com.SPE.historytrails.History_Trails.service.ObjectService;

import static org.assertj.core.api.Assertions.assertThat;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.put;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.content;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.status;
import static org.springframework.security.test.web.servlet.request.SecurityMockMvcRequestPostProcessors.csrf;



import com.fasterxml.jackson.databind.ObjectMapper;
import org.junit.jupiter.api.Test;
import org.mockito.Mockito;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.WebMvcTest;
import org.springframework.boot.test.mock.mockito.MockBean;
import org.springframework.test.web.servlet.MockMvc;
import org.springframework.test.web.servlet.MvcResult;

import java.util.ArrayList;
import java.util.List;



@WebMvcTest(ObjectController.class)
public class ObjectControllerTest {


    @Autowired
    private MockMvc mockMvc;

    @Autowired
    private ObjectMapper objectMapper;

    @MockBean
    private ObjectService objectService;


    @Test
    public void testListObjects() throws Exception {
        List<Objects> listObjects = new ArrayList<>();
        listObjects.add(new Objects("1", 1));
        listObjects.add(new Objects("2", 2));
        listObjects.add(new Objects("3", 3));
        listObjects.add(new Objects("4", 4));

        Mockito.when(objectService.list()).thenReturn(listObjects);
        String url = "/objects/list";
        MvcResult mvcResult = this.mockMvc.perform(get(url)).andExpect(status().isOk()).andReturn();

        String actualJsonResponse = mvcResult.getResponse().getContentAsString();
        String expectedJsonResponse = objectMapper.writeValueAsString(listObjects);

        assertThat(actualJsonResponse).isEqualTo(expectedJsonResponse);
    }


    @Test
    public void testGetLikes() throws Exception {
        Objects object1 = new Objects("1", 1);


        Mockito.when(objectService.listOne("1")).thenReturn(object1);

        String url = "/objects";

        MvcResult mvcResult = this.mockMvc.perform(get(url).param("id", "1")).andExpect(status().isOk()).andReturn();
        String actualJsonResponse = mvcResult.getResponse().getContentAsString();
        String expectedJsonResponse = objectMapper.writeValueAsString(object1);
        assertThat(actualJsonResponse).isEqualTo(expectedJsonResponse);
    }

    @Test
    public void testGetLikesNotExist() throws Exception {
        Objects object1 = new Objects("1", 1);

        Mockito.when(objectService.listOne("1")).thenReturn(object1);

        String url = "/objects";

        MvcResult mvcResult = this.mockMvc.perform(get(url).param("id", "2")).andExpect(status().isOk()).andReturn();
        String actualJsonResponse = mvcResult.getResponse().getContentAsString();
        String expectedJsonResponse = "";
        assertThat(actualJsonResponse).isEqualTo(expectedJsonResponse);
    }


    @Test
    public void testPutLikes() throws Exception {

        Objects object1 = new Objects("1", 5);
        Mockito.when(objectService.listOne("1")).thenReturn(object1);

        String url = "/objects";
        String expectedJsonResponse = objectMapper.writeValueAsString(object1);


        this.mockMvc.perform(
                put(url)
                .param("id", "1")
                .contentType("application/json")
                .content(objectMapper.writeValueAsString(object1))
                .with(csrf()))
                .andExpect(status().isOk())
                .andExpect(content().string(expectedJsonResponse));
    }

    @Test
    public void testInvalidCSRFToken() throws Exception {

        Objects object1 = new Objects("1", 5);
        Mockito.when(objectService.listOne("1")).thenReturn(object1);

        String url = "/objects";


        this.mockMvc.perform(
                put(url)
                        .param("id", "1")
                        .contentType("application/json")
                        .content(objectMapper.writeValueAsString(object1)))
                .andExpect(status().is(403));
    }



}

