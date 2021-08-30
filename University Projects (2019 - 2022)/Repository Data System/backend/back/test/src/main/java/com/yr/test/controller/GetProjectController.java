package com.yr.test.controller;

import com.yr.test.entity.repoLangEntity;
import demo.*;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import utils.R;

import java.io.IOException;
import java.util.*;

@RestController
public class GetProjectController {

    private String test_url = "https://github.com/spe-uob/";
    private String test_token = "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";


    /**
     * [Newly add api]: Get issues from specific project name from github API with [url] and [token]
     * @param url
     * @return
     * @throws Exception
     */
    @RequestMapping("/getIssues")
    public R getIssueFromURL(@RequestParam("url") String url) throws Exception {
        IssuesDemo issuesDemo = new IssuesDemo();
        ArrayList<Response> issues = issuesDemo.getIssues(test_url + url, test_token);

        return R.ok().put("data", issues);
    }


    //getProj
    @RequestMapping("/getAllProjects")
    public R getProj(@RequestParam("url") String url) throws IOException {
        System.out.println(test_url+url);
        RepoDemo repoDemo = new RepoDemo();
        //call getProj
        ArrayList<Integer> proj = repoDemo.getProj(test_url+url, test_token);
        //return data
        return R.ok().put("data", proj);
    }

    //getAllUser
    @RequestMapping("/getAllUser")
    public R getAllUser(@RequestParam("url") String url) throws IOException {

        RepoDemo repoDemo = new RepoDemo();

        ArrayList<Result> user = repoDemo.getUser(test_url+url, test_token);
        System.out.println(user);
        return R.ok().put("data", user);
    }


    //getRepoLang
    @RequestMapping("/getRepoLang")
    public R getRepoLang(@RequestParam("url") String url) throws IOException {

        RepoLang lang = new RepoLang();

        HashMap<String, Integer> repoLang = lang.getRepoLang(test_url+url, test_token);

        List<repoLangEntity> repoLangEntities = getkey3(repoLang);

        return R.ok().put("data",repoLangEntities);
    }

    //Use the returned set which the frontend can directly use the data returned by the background
    public static List<repoLangEntity> getkey3(HashMap<String, Integer> repoLang) {
        List<repoLangEntity> list = new ArrayList<>();
        Set<Map.Entry<String, Integer>> entries = repoLang.entrySet();

        for (Map.Entry<String, Integer> entry : entries) {
            repoLangEntity repoLangEntity = new repoLangEntity();
            repoLangEntity.setName(entry.getKey());
            repoLangEntity.setValue(entry.getValue());
            list.add(repoLangEntity);
        }
        return list;
    }
}