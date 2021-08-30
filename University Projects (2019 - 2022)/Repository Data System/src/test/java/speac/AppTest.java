package speac;

import com.microsoft.graph.models.TeamsTab;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.Properties;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;

public class AppTest {

    @BeforeEach
    void setUp() {
    }


    final App app = new App();
    final Properties oAuthProperties = new Properties();


    @Test
    public void AppContext() {

        assertNotNull(app);
    }

    @Test
    public void AppAccessToken(){
        final String scope = "ChannelMessage.Send,TeamsTab.ReadWrite.All,Group.ReadWrite.All,Directory.ReadWrite.All";
        final List<String> appScopes = Arrays.asList(scope.split(","));
        Graph.initializeGraphAuth("4e33241f-a3ea-4fe3-a404-616a7e490014",appScopes);
        final String accessToken = Graph.getUserAccessToken();
        assertNotNull(accessToken);
    }

    @Test
    public void AppID() {
    try {
            oAuthProperties.load(App.class.getResourceAsStream("oAuth.properties"));
            } catch (IOException e) {
                System.out.println("Unable to read OAuth configuration. Make sure you have a properly formatted oAuth.properties file. See README for details.");
                return;
            }
    assertEquals("4e33241f-a3ea-4fe3-a404-616a7e490014", oAuthProperties.getProperty("app.id"));     
}

     @Test
     public void AppScopes() {
        try {
            oAuthProperties.load(App.class.getResourceAsStream("oAuth.properties"));
            } catch (IOException e) {
                System.out.println("Unable to read OAuth configuration. Make sure you have a properly formatted oAuth.properties file. See README for details.");
                return;
            }
        String appScope = "ChannelMessage.Send,TeamsTab.ReadWrite.All,Group.ReadWrite.All,Directory.ReadWrite.All";
        List<String> listAppScope = Arrays.asList(appScope.split(","));

        assertEquals(listAppScope,Arrays.asList(oAuthProperties.getProperty("app.scopes").split(",")) );
        }

     @Test
    public void testListTab(){
        final String scope = "ChannelMessage.Send,TeamsTab.ReadWrite.All,Group.ReadWrite.All,Directory.ReadWrite.All";
        final List<String> appScopes = Arrays.asList(scope.split(","));
        Graph.initializeGraphAuth("4e33241f-a3ea-4fe3-a404-616a7e490014",appScopes);//initialise graph
        List<TeamsTab> tabs = Graph.listTabs();
        assertEquals(tabs.get(0).displayName, "Wiki" );
     }

     @Test
    public void testAddTab(){
         final String scope = "ChannelMessage.Send,TeamsTab.ReadWrite.All,Group.ReadWrite.All,Directory.ReadWrite.All";
         final List<String> appScopes = Arrays.asList(scope.split(","));
         Graph.initializeGraphAuth("4e33241f-a3ea-4fe3-a404-616a7e490014",appScopes);//initialise graph
         final String displayName = "Test purpose";
         final String websiteUrl = "https://www.google.com/?client=safari";
         final String contentUrl = "https://www.google.com/?client=safari";
         Graph.addTab(websiteUrl, contentUrl, displayName);
         List<TeamsTab> tabs = Graph.listTabs();
         assertEquals(tabs.get(2).displayName, "Test purpose");
         assertEquals(Objects.requireNonNull(tabs.get(2).configuration).websiteUrl,"https://www.google.com/?client=safari");
     }

    @Test
    public void testDeleteTab(){
        final String scope = "ChannelMessage.Send,TeamsTab.ReadWrite.All,Group.ReadWrite.All,Directory.ReadWrite.All";
        final List<String> appScopes = Arrays.asList(scope.split(","));
        Graph.initializeGraphAuth("4e33241f-a3ea-4fe3-a404-616a7e490014",appScopes);//initialise graph
        List<TeamsTab> tabs = Graph.listTabs();
        Graph.deleteTab(tabs.get(2).id);
        List<TeamsTab> tabs2 = Graph.listTabs();
        Integer numberOfTabs = tabs2.size();
        assertEquals(numberOfTabs,2);
    }
}