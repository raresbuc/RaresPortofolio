package com.SPE.historytrails.History_Trails;


import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;


@Controller
public class WebController {

    @GetMapping(value = "/")
    public String index(){ return "index"; }
    @GetMapping(value = "/MShed")
    public String MShed(){
        return "MShed";
    }
    @GetMapping(value = "/BristolArtGallery")
    public String BristolArtGallery(){ return "BristolArtGallery"; }
    @GetMapping(value = "/popular")
    public String popular() {return "popular";}
    @GetMapping(value = "/favourites")
    public String favourites() {return "favourites";}
    @GetMapping(value = "/Gallery2")
    public String gallery2(){ return "Gallery2"; }
    @GetMapping(value = "/Gallery3")
    public String gallery3(){
        return "Gallery3";
    }
    @GetMapping(value = "/Gallery4")
    public String gallery4(){
        return "Gallery4";
    }
    @GetMapping(value = "/Gallery5")
    public String gallery5(){
        return "Gallery5";
    }
    @GetMapping(value = "/more")
    public String more(){
        return "more";
    }
}

