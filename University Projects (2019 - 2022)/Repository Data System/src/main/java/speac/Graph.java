package speac;

import com.azure.identity.DeviceCodeCredential;
import com.azure.identity.DeviceCodeCredentialBuilder;
import com.google.gson.JsonPrimitive;
import com.microsoft.graph.authentication.TokenCredentialAuthProvider;
import com.microsoft.graph.logger.DefaultLogger;
import com.microsoft.graph.logger.LoggerLevel;
import com.microsoft.graph.models.*;
import com.microsoft.graph.requests.GraphServiceClient;
import com.microsoft.graph.requests.TeamsTabCollectionPage;
import okhttp3.Request;

import java.io.IOException;
import java.net.URL;
import java.util.List;
import java.util.Objects;


public class Graph {

    private static final String teamsId = "e30b8961-a940-4c5f-ab8b-70b8705e85fa";
    private static final String channelId = "19:T99WnpX5SqGGUG7MmIr1nsG_ltuo3R6h2GYbLFNaoxk1@thread.tacv2";
    private static GraphServiceClient<Request> graphClient = null;
    private static TokenCredentialAuthProvider authProvider = null;

    public static void initializeGraphAuth(String applicationId, List<String> scopes) {

        // Create the auth provider
        final DeviceCodeCredential credential = new DeviceCodeCredentialBuilder().clientId(applicationId).challengeConsumer(challenge -> System.out.println(challenge.getMessage())).build();
        authProvider = new TokenCredentialAuthProvider(scopes, credential);

        // Create default logger to only log errors
        DefaultLogger logger = new DefaultLogger();
        logger.setLoggingLevel(LoggerLevel.ERROR);

        // Build a Graph client
        graphClient = GraphServiceClient.builder().authenticationProvider(authProvider).logger(logger).buildClient();
    }


    public static String getUserAccessToken() {
        try {
            URL meUrl = new URL("https://graph.microsoft.com/v1.0/me");
            return authProvider.getAuthorizationTokenAsync(meUrl).get();
        } catch (Exception ex) {
            return null;
        }
    }


    public static User getUser() {
        if (graphClient == null)
            throw new NullPointerException("Graph client has not been initialized. Call initializeGraphAuth before calling this method");

        return graphClient.me().buildRequest().select("displayName,mailboxSettings").get();
    }


    public static List<TeamsTab> listTabs() {
        GraphServiceClient graphClient = GraphServiceClient.builder().authenticationProvider(authProvider).buildClient();

        TeamsTabCollectionPage tabs = Objects.requireNonNull(graphClient.teams(teamsId).channels(channelId).tabs().buildRequest()).expand("teamsApp").get();
        assert tabs != null;

        return tabs.getCurrentPage();
    }


    public static void deleteTab(String tabId) {
        GraphServiceClient graphClient = GraphServiceClient.builder().authenticationProvider(authProvider).buildClient();

        Objects.requireNonNull(graphClient.teams(teamsId).channels(channelId).tabs(tabId).buildRequest().delete());
    }


    public static void addTab(String webURL, String contentURL, String name) {
        GraphServiceClient graphClient = GraphServiceClient.builder().authenticationProvider(authProvider).buildClient();

        TeamsTab teamsTab = new TeamsTab();
        teamsTab.displayName = name;
        teamsTab.additionalDataManager().put("teamsApp@odata.bind", new JsonPrimitive("https://graph.microsoft.com/beta/appCatalogs/teamsApps/com.microsoft.teamspace.tab.web"));
        TeamsTabConfiguration configuration = new TeamsTabConfiguration();
        configuration.entityId = null;
        configuration.contentUrl = contentURL;
        configuration.websiteUrl = webURL;
        configuration.removeUrl = null;
        teamsTab.configuration = configuration;

        Objects.requireNonNull(graphClient.teams(teamsId).channels(channelId).tabs().buildRequest()).post(teamsTab);
    }


    public static void sendMessage(String message) {
        GraphServiceClient graphClient = GraphServiceClient.builder().authenticationProvider(authProvider).buildClient();

        ChatMessage chatMessage = new ChatMessage();
        ItemBody body = new ItemBody();
        body.content = message;
        chatMessage.body = body;

        Objects.requireNonNull(graphClient.teams(teamsId).channels(channelId).messages().buildRequest()).post(chatMessage);
    }


    public static void updateTab(String name, String appId) {
        GraphServiceClient graphClient = GraphServiceClient.builder().
                authenticationProvider(authProvider).buildClient();

        TeamsTab teamsTab = new TeamsTab();
        teamsTab.displayName = name;

        Objects.requireNonNull(graphClient.teams(teamsId).channels(channelId).tabs(appId)
                .buildRequest().patch(teamsTab));
    }
}