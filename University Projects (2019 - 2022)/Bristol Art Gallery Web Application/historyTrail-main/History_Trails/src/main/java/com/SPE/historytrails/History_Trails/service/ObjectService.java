package com.SPE.historytrails.History_Trails.service;


import com.SPE.historytrails.History_Trails.domain.Objects;
import com.SPE.historytrails.History_Trails.repository.ObjectRepository;
import org.springframework.stereotype.Service;
import javax.persistence.EntityNotFoundException;
import java.util.List;

@Service
public class ObjectService {

    private final ObjectRepository objectRepository;

    public ObjectService(ObjectRepository objectRepository) {
        this.objectRepository = objectRepository;
    }

    public Iterable<Objects> list() {
        return objectRepository.findAll();
    }

    public Objects listOne(String id) {
         return objectRepository.findById(id)
                .orElseThrow(() -> new EntityNotFoundException(id));
    }
    public void save(Objects objects) { objectRepository.save(objects);
    }



    public void save(List<Objects> objects) {
        objectRepository.saveAll(objects);
    }
}
