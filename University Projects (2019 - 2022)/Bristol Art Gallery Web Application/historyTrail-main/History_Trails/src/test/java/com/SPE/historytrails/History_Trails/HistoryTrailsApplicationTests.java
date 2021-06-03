package com.SPE.historytrails.History_Trails;

import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;

import static org.assertj.core.api.Assertions.assertThat;

@SpringBootTest
public class HistoryTrailsApplicationTests {

	/**	check the application context can start**/
	 @Test
	void contextLoads() {
	}


	@Autowired
	WebController webcontroller;


	/**	check the application context is creating controller**/
	@Test
	public void contextLoadsController() {
		assertThat(webcontroller).isNotNull();
	}


	@Test
	void htmlRendering(){
		assert(webcontroller.index().equals("index"));
		assert(webcontroller.MShed().equals("MShed"));
		assert(webcontroller.BristolArtGallery().equals("BristolArtGallery"));
		assert(webcontroller.popular().equals("popular"));
		assert(webcontroller.favourites().equals("favourites"));
		assert(webcontroller.gallery2().equals("Gallery2"));
		assert(webcontroller.gallery3().equals("Gallery3"));
		assert(webcontroller.gallery4().equals("Gallery4"));
		assert(webcontroller.gallery5().equals("Gallery5"));
		assert(webcontroller.more().equals("more"));

	}

}
