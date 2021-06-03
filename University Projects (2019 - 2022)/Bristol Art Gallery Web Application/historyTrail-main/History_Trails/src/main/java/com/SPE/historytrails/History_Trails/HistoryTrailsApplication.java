package com.SPE.historytrails.History_Trails;

import com.SPE.historytrails.History_Trails.domain.Objects;
import com.SPE.historytrails.History_Trails.service.ObjectService;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.Bean;

import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.Base64;
import java.util.List;




@SpringBootApplication
public class HistoryTrailsApplication {

	public static void main(String[] args) {
		SpringApplication.run(HistoryTrailsApplication.class, args);
	}


	public static JSONArray initialiseLikes() {

		JSONArray likesList = new JSONArray();

		JSONObject galleryList = requestJSON("https://opendata.bristol.gov.uk/api/records/1.0/search/?dataset=open-data-gallery-3-european-old-masters&q=&rows=115&sort=-order_of_appearance&facet=medium&facet=object_type","","");

		Iterable<JSONObject> objects = (Iterable<JSONObject>) galleryList.get("records");

		//Iterate over gallery array
		objects.forEach(object -> {

			Object recordid = object.get("recordid");
			JSONObject objectLikes = new JSONObject();
			objectLikes.put("recordid", recordid);
			objectLikes.put("likes", 0);
			likesList.add(objectLikes);
		});

		return likesList;

	}


	public static void createLikesfile(){

		JSONArray likesList = initialiseLikes();
		try (FileWriter file = new FileWriter("src/main/resources/json/likes.json")) {
			file.write(likesList.toString());
			file.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}


	private static JSONObject requestJSON(String urlString, String username, String key)
	{
		try {
			URL url = new URL(urlString);
			byte[] keyBytes = (username + ":" + key).getBytes(StandardCharsets.UTF_8);
			String encodedKey = Base64.getEncoder().encodeToString(keyBytes);
			HttpURLConnection connection = (HttpURLConnection) url.openConnection();
			connection.setRequestMethod("GET");
			connection.setDoOutput(true);
			connection.setRequestProperty ("Authorization", "Basic " + encodedKey);
			InputStream stream = connection.getInputStream();
			JSONParser parser = new JSONParser();
			return (JSONObject)parser.parse(new InputStreamReader(stream, StandardCharsets.UTF_8));
		} catch(IOException | ParseException uee) { return null; }
	}

	@Bean
	CommandLineRunner runner(ObjectService objectService){
		return args -> {

			createLikesfile();


			ObjectMapper mapper = new ObjectMapper();
			TypeReference<List<Objects>> typeReference = new TypeReference<>() {};
			InputStream targetStream = TypeReference.class.getResourceAsStream("/json/likes.json");

			try {
				List<Objects> objects = mapper.readValue(targetStream,typeReference);
				objectService.save(objects);

				System.out.println("Objects Saved!");
			} catch (IOException e){
				System.out.println("Unable to save objects: " + e.getMessage());
			}
		};
	}
}
