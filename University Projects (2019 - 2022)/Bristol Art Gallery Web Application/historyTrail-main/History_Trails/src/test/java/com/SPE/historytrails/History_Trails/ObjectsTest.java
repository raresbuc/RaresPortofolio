package com.SPE.historytrails.History_Trails;

import com.SPE.historytrails.History_Trails.domain.Objects;
import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;
import static org.assertj.core.api.Assertions.assertThat;



@SpringBootTest
public class ObjectsTest {

    @Test
   public void testObjectGetterSetter(){
        Objects newObject = new Objects();
        newObject.setLikes(5);
        assertThat(newObject.getLikes() == 5);
    }

    @Test
    public void testNegativeLikes(){
        Objects newObject = new Objects();
        newObject.setLikes(10);
        newObject.setLikes(-1);
        assertThat(newObject.getLikes() == 10);
    }

}
