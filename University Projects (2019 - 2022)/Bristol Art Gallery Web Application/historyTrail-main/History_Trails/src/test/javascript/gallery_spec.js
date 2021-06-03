describe('Testing API...', function() {

    // let result = [];
    beforeEach(function(done) {
        setTimeout(function() {
            // do some stuff
            done();
        }, 500);
    });

    it('Works API URL - status OK', async function(done) {
        let url = 'https://opendata.bristol.gov.uk/api/records/1.0/search/?dataset=open-data-gallery-3-european-old-masters&q=&rows=115&sort=-order_of_appearance&facet=medium&facet=object_type';
        const request = await fetch(url);
        expect(request.status).toBe(200);
        done();
    });


    it('Fetches all the objects', async function(done) {
        const result = await fetchObjects();
        expect(result.length).toBe(115);
        done();
    });

    it('Extracts all relevant information about the objects', async function(done) {
        const result = await fetchObjects();
        let objectsGallery2 = [];
        let objectsGallery3 = [];
        let objectsGallery4 = [];
        let objectsGallery5 = [];
        for (var i = 0; i < 115; i++) {
            if (result[i].fields.gallery === "Gallery 2") {
                objectsGallery2.push(result[i]);
            }
            else if (result[i].fields.gallery === "Gallery 3 - European Old Masters") {
                objectsGallery3.push(result[i]);
            }
            else if (result[i].fields.gallery === "Gallery 4") {
                objectsGallery4.push(result[i]);
            }
            else if (result[i].fields.gallery === "Gallery 5 - Modern Art") {
                objectsGallery5.push(result[i]);
            }

        }
        expect(objectsGallery2[0].recordid).toBe("b2b810de148a48a51ddd54ae124389c6831ec3bc");
        expect(objectsGallery2[0].fields.title_of_object).toBe("Young Italian Girl");
        expect(objectsGallery2[0].fields.artist).toBe("Thomas COUTURE");
        expect(objectsGallery2[0].fields.artist_s_birth_death).toBe("1815 – 1879");
        expect(objectsGallery2[0].fields.year_of_creation).toBe("about 1877");
        expect(objectsGallery2[0].fields.label).toBe("Painted in broad brushstrokes, Couture has captured the head and shoulders of a young, dark haired girl in traditional Italian folk costume. She has turned her face to the right and gazes into the distance. Her long coral bead necklace with its gilt cross pendant stands out from her white shirt. Striving to achieve recognition and success at the Paris Salon, Thomas Couture produced a number of large, complex compositions in the grand tradition. By contrast, this more intimate painting demonstrates the more spontaneous side of his nature. It is one of a number of studies which Couture made of the strolling Italian players who passed by his home at Villiers-le-Bel in 1877. Couture was a popular and influential teacher in Paris and encouraged his pupils to retain the qualities of the sketch, such as pure colour and fresh brushwork, in their finished paintings. Among his pupils were Edouard Manet, Pierre Puvis de Chavannes, and several American artists.");

        expect(objectsGallery3[0].recordid).toBe("c9186b9a3d0b8fd11ebbfd493ca2ec292dfad840");
        expect(objectsGallery3[0].fields.title_of_object).toBe("Flagon");
        expect(objectsGallery3[0].fields.artist).toBe("Maker's mark: F over W, London");
        expect(objectsGallery3[0].fields.artist_s_birth_death).toBe(undefined);
        expect(objectsGallery3[0].fields.year_of_creation).toBe("1620 - 1621");
        expect(objectsGallery3[0].fields.label).toBe("Dedicatory inscription in the same band as those on ewer and bowl");


        expect(objectsGallery4[0].recordid).toBe("7454e2c9c159d81666f4acbcf40ce9d4e031f975");
        expect(objectsGallery4[0].fields.title_of_object).toBe("Capriccio: The Lagoon, Venice");
        expect(objectsGallery4[0].fields.artist).toBe("Bernardo Bellotto");
        expect(objectsGallery4[0].fields.artist_s_birth_death).toBe("1721 - 1780");
        expect(objectsGallery4[0].fields.year_of_creation).toBe("About 1743");
        expect(objectsGallery4[0].fields.label).toBe("A quiet, watery scene is populated with only a small number of solemn figures and buildings. The damp climate of the vast Venetian Lagoon is tangible in the crumbling plaster of the humble boathouses and shipyards in the foreground. Beyond, the Tower of Malghera and the skyline of Venice appear in the slightly hazy sunlight. The artist and his uncle Giovanni Antonio Canal, commonly called Canaletto, collected motifs from the atmospheric Lagoon on a trip to Padua in 1741. This imaginary or capriccio painting and the Tower of Malghera are in fact based on compositions by Canaletto, but the young artist was soon to lead his own successful career as court painter in Dresden, Vienna and Warsaw.");

        expect(objectsGallery5[0].recordid).toBe("56f4856fee619e14419f5a2ece50d5a6ac2a3964");
        expect(objectsGallery5[0].fields.title_of_object).toBe("England 1968");
        expect(objectsGallery5[0].fields.artist).toBe("Richard Long");
        expect(objectsGallery5[0].fields.artist_s_birth_death).toBe("Born 1945");
        expect(objectsGallery5[0].fields.year_of_creation).toBe("1968");
        expect(objectsGallery5[0].fields.label).toBe("Since his art foundation course Long had been experimenting with ephemeral forms of sculpture and gentle interventions in the landscape. The most instantaneous was to trace a path by rolling a snowball across the snow-covered Bristol Downs. For England Long picked daisies in a field, marking out the cross of the English flag. Today more than ever Long's reversible marks on the landscape have pointed resonance as we recognise the climate emergency caused by humanity.");

        done();
    });

    it('Gallery 2', async function (done) {
        const result = await fetchObjects();
        let objectsGallery2 = [];
        for (var i = 0; i < 115; i++) {
            if (result[i].fields.gallery === "Gallery 2") {
                objectsGallery2.push(result[i]);
            }
        }
        expect(objectsGallery2.length).toBe(31);
        done();
    });

    it('Gallery 3 - European Old Masters', async function (done) {
        const result = await fetchObjects();
        let objectsGallery3 = [];
        for (var i = 0; i < 115; i++) {
            if (result[i].fields.gallery === "Gallery 3 - European Old Masters") {
                objectsGallery3.push(result[i]);
            }
        }
        expect(objectsGallery3.length).toBe(44);
        done();
    });

    it('Gallery 4', async function (done) {
        const result = await fetchObjects();
        let objectsGallery4 = [];
        for (var i = 0; i < 115; i++) {
            if (result[i].fields.gallery === "Gallery 4") {
                objectsGallery4.push(result[i]);
            }
        }
        expect(objectsGallery4.length).toBe(22);
        done();
    });

    it('Gallery 5 - Modern Art', async function (done) {
        const result = await fetchObjects();
        let objectsGallery5 = [];
        for (var i = 0; i < 115; i++) {
            if (result[i].fields.gallery === "Gallery 5 - Modern Art") {
                objectsGallery5.push(result[i]);
            }
        }
        expect(objectsGallery5.length).toBe(18);
        done();
    });

    it('Works all links to additional information', async function (done) {
        const result = await fetchObjects();
        let urls = [];
        for (var i = 0; i < 115; i++) {
            var url = result[i].fields.link_to_additional_information;
            if (url) {
                urls.push(url);
            }
        }
        for (var i = 0; i < urls.length; i++) {
            const request = await fetch(urls[i]);
            expect(request.status).toBe(200);
        }
        done();
    })
});
