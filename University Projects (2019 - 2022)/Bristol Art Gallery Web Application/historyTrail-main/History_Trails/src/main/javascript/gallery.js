
function makeClickable(url) {
    return String.prototype.link ? "Additional Information".link(url) : '<a href="'+url+'">'+url+'</a>';
}

function testLink(string) {
    let url;

    try {
        url = new URL(string);
    } catch (_) {
        return false;
    }
    return url.protocol === "http:" || url.protocol === "https:";

}


const charactersList = document.getElementById('charactersList');
const searchBar = document.getElementById('searchBar');
let hpCharacters = [];

searchBar.addEventListener('keyup', (e) => {
    const searchString = e.target.value.toLowerCase();

    const filteredCharacters = hpCharacters.filter((obj) => {
        var artistName = (obj.fields.artist) ? obj.fields.artist : "";

        return (
            obj.fields.title_of_object.toLowerCase().includes(searchString) ||
            artistName.toLowerCase().includes(searchString)
        );
    });
    displayCharacters(filteredCharacters);
});

async function fetchObjects() {
    let url = 'https://opendata.bristol.gov.uk/api/records/1.0/search/?dataset=open-data-gallery-3-european-old-masters&q=&rows=115&sort=-order_of_appearance&facet=medium&facet=object_type';
    try {
        let res = await fetch(url);
        return (await res.json()).records;
    } catch (error) {
        console.log(error);
    }
}


const loadCharacters = async () => {
    try {
        hpCharacters = (await fetchObjects());
        displayCharacters(hpCharacters);
    } catch (err) {
        console.error(err);
    }
};



const displayCharacters = (out) => {
    const htmlString = out
        .map((obj) => {
            if (obj.fields.gallery === gallery) {
                let img;
                let favClass;
                if (!obj.fields.image_of_object) {
                    img = obj.recordid;
                } else {
                    img = obj.fields.image_of_object.id;
                }
                favClass = '<i class="heart far fa-heart"></i>';
                if (getCookie(obj.recordid)){
                    favClass = '<i class="heart fas fa-heart"></i>';

                }

                const artistBirthDeath = (obj.fields.artist_s_birth_death) ? obj.fields.artist_s_birth_death : "";
                let linkToAdditionalInformation = (obj.fields.link_to_additional_information && testLink(obj.fields.link_to_additional_information)) ? makeClickable(obj.fields.link_to_additional_information) : "";
                return `
                        <div class="object">
                            <div class = "object-top">
                                <span id = ${obj.recordid} onclick="fav(this.id)">${favClass} </span>
                                <h2 class="title-of-object"> <strong>${obj.fields.title_of_object}</strong></h2>
                                <h3 class="name-of-artist"> <strong>${obj.fields.artist}</strong></h3>
                                <h4 class="artist-birth-death"> <strong>${artistBirthDeath}</strong></h4>
                            </div>
                            <img class="object-image" alt = "${obj.fields.title_of_object}" src="/images/${img}.jpg"/>
                            <div class = "object-info">
                                ${obj.fields.label}
                                <p> ${linkToAdditionalInformation}</p>
                            </div>
                        </div>
                    `
            }
        })
        .join('');
    charactersList.innerHTML = htmlString;
};


window.onload = function() {
    loadCharacters().then(r => cookieConsent());
}

module.exports = fetchObjects()