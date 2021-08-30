# GHAM Mock Library Instructions

## Description
A mock library provided for the SPEAC team. This library provides methods to get a set of metrices back, both take in a SPE GitHub repository and a token as parameters. 

## Token
You should create a personal access token for authentication to GitHub when using GitHub API.

If you have a spe-uob owner token, you could pass it as a parameter to get metrics for all existing spe project urls. Otherwise, please generate a new token. 

[Creating a personal access token](https://docs.github.com/en/github/authenticating-to-github/keeping-your-account-and-data-secure/creating-a-personal-access-token)

## Usage
### Import GHAMdemo.jar

Warning: jdk15 is preferred, any jdk version lower than jdk15 is not reconmmended.

Please look at [Import Instructions](ImportInstructions.md) if you have any problems.
### Method Description

`getProj()` method returns lines of code changed per week per project in an arraylist.

`getUser()` method returns all contributor commit activity in an arraylist of 'Result's.

Result is a Class that includes user commit information:
* contributor username
* total number of commits per user
* lines of code changed per week per user

where lines of code changed per week per user is returned in an arraylist.

`getRepoLnag()` method returns language used in a repositoy.

`getPercentage()` method prints out the percentage of language used in a repository.

### Small Example
``` Java
public static void main(String[] args) throws IOException {
    String test_url = "https://github.com/spe-uob/2021-R-SPEAC";
    String test_token = "xxxxxxxxx";
    RepoDemo test = new RepoDemo();
    RepoLang lang = new RepoLang();

    ArrayList<Integer> testProj = test.getProj(test_url, test_token);
    ArrayList<Result> testUser = test.getUser(test_url, test_token);

    HashMap<String,Integer> testLang = lang.getRepoLang(test_url,test_token);
    lang.getPercentage(testLang);
}
```

