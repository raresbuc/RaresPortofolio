package com.SPE.historytrails.History_Trails.controller;

import com.SPE.historytrails.History_Trails.domain.Objects;
import com.SPE.historytrails.History_Trails.service.ObjectService;
import org.springframework.web.bind.annotation.*;


@RestController
@RequestMapping("/objects")
public class ObjectController {

    private final ObjectService objectService;

    public ObjectController(ObjectService objectService) {
        this.objectService = objectService;
    }

    @GetMapping("/list")
    public Iterable<Objects> list() {
        return objectService.list();
    }

    @GetMapping(value="")
    public Objects getLikes(@RequestParam()String id )  {
        return objectService.listOne(id);
    }


    @PutMapping(value="")
    public Objects updateLikes(@RequestParam()String id, @RequestBody Objects newObject )  throws Exception{
        if ( !id.equals(newObject.getRecordid())){throw new Exception("Ids don't match");}

        Objects object = objectService.listOne(id);
        object.setLikes(newObject.getLikes());
        objectService.save(object);

        return objectService.listOne(id);
    }

}
