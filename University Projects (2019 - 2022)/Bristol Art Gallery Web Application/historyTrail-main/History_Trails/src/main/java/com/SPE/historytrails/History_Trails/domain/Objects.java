package com.SPE.historytrails.History_Trails.domain;

import lombok.Data;

import javax.persistence.*;

@Data
@Entity
public class Objects {

    @Id
    private String recordid;
    private int likes;


    public Objects(String recordid, int likes) {
        this.recordid = recordid;
        this.likes = likes;
    }


    public Objects(String recordid) {
        this.recordid = recordid;
    }



    /**
     * get the number of likes of object
     * @return likes
     */
    public int getLikes() {
       return likes;
    }


    /**
     * update likes of object.
     * @param likes number of likes to set Objects to
     */
    public void setLikes(int likes) {
        if (likes >= 0) this.likes = likes;
    }


    /**
     * get the number of recordid of object
     * @return likes
     */
    public String getRecordid() {
        return recordid;
    }


    public Objects() {}





}

