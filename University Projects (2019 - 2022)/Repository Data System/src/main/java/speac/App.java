package speac;

import com.microsoft.graph.models.TeamsTab;
import com.microsoft.graph.models.User;

import java.io.IOException;
import java.net.URL;
import java.util.*;


public class App {

    public static void main(String[] args) throws IOException, InterruptedException {
        System.out.println("Repo Data");
        System.out.println();

        // Load OAuth settings
        final Properties oAuthProperties = new Properties();
        try {
            oAuthProperties.load(App.class.getResourceAsStream("oAuth.properties"));
        } catch (IOException e) {
            System.out.println("Unable to read OAuth configuration. Make sure you have a properly formatted oAuth.properties file. See README for details.");
            return;
        }

        final String appId = oAuthProperties.getProperty("app.id");
        final List<String> appScopes = Arrays.asList(oAuthProperties.getProperty("app.scopes").split(","));

        // Initialize Graph with auth settings
        Graph.initializeGraphAuth(appId, appScopes);
        final String accessToken = Graph.getUserAccessToken();

        // Greet the user
        User user = Graph.getUser();
        System.out.println("Welcome " + user.displayName);
        assert user.mailboxSettings != null;
        System.out.println("Time zone: " + user.mailboxSettings.timeZone);
        System.out.println();

        Scanner input = new Scanner(System.in);

        int choice = -1;

        while (choice != 0) {
            System.out.println("Please choose one of the following options:");
            System.out.println("0. Exit");
            System.out.println("1. Display access token");
            System.out.println("2. Send Message to Teams");
            System.out.println("3. Create Tab");
            System.out.println("4. List all Tabs");
            System.out.println("5. Select tab to delete ");
            System.out.println("6. Update tab information");

            try {
                choice = input.nextInt();
            } catch (InputMismatchException ex) {
                // Skip over non-integer input
            }

            input.nextLine();

            // Process user choice
            switch (choice) {
                case 0:
                    // Exit the program
                    System.out.println("Goodbye...");
                    break;

                case 1:
                    // Display access token
                    System.out.println("Access token: " + accessToken);
                    break;

                case 2:
                    // Create a new event
                    String message = getMessage(input);
                    Graph.sendMessage(message);
                    break;

                case 3:
                    addTab(input);
                    System.out.println("Remember to delete tab if testing");
                    break;

                case 4:
                    List<TeamsTab> tabs = Graph.listTabs();
                    listTabs(tabs);
                    break;

                case 5:
                    tabToDelete(input,Graph.listTabs());
                    break;

                case 6:
                    updateTab(input);
                    break;

                default:
                    System.out.println("Invalid choice");
            }
        }
        input.close();
    }


    private static void tabToDelete(Scanner input, List<TeamsTab> tabs){
        System.out.println("Select tab to delete");
        listTabs(tabs);
        String choice = "";
        while (choice.isEmpty() && Integer.parseInt(choice) >= 0){
            choice = input.nextLine();
        }

        Graph.deleteTab(tabs.get(Integer.parseInt(choice) - 1).id);
    }


    public static void listTabs(List<TeamsTab> tabs) {
        for (int i = 0; i < tabs.size(); i++) {
            System.out.println(i + 1 + ". " + tabs.get(i).displayName + "\t" + "App-id:" + tabs.get(i).id);
        }
    }


    private static String getMessage(Scanner input) {
        String subject = "";
        while (subject.isEmpty()) {
            System.out.print("Message to send: ");
            subject = input.nextLine();
        }
        return subject;
    }


    private static void addTab(Scanner input){
        String websiteURL;
        String contentURL;
        String name = "";

        while(name.isEmpty()){
            System.out.print("Display name on the tab");
            name = input.nextLine();
        }

        while(true){
            System.out.println("\nPlease enter website url");
            websiteURL = input.nextLine();
            contentURL = websiteURL;

            if(!isValidUrl(websiteURL)){
                System.out.print("\nIs not a valid URL");

            }else{
                break;
            }
        }

        Graph.addTab(websiteURL, contentURL,name);
    }


    private static boolean isValidUrl(String url){
        try{
            new URL(url).toURI();
            return true;
        }catch(Exception e){
            //if execution generate then return False
            return false;
        }
    }


    private static void updateTab(Scanner input){
        String name ="";
        String tabsID ="";

        while (name.isEmpty()){
            System.out.print("\nPlease enter display name for the tab");
            name = input.nextLine();
        }

        while (tabsID.isEmpty()){
            System.out.print("\nPlease enter tab id ");
            tabsID = input.nextLine();
        }

        Graph.updateTab(name,tabsID);
    }

}