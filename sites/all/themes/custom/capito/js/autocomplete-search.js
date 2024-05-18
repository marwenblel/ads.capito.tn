function handleKeywordChange() {
    keyword = document.getElementById("search-ad").value.trim().substring(0, 20);;
   // keyword.value = keyword.value.toUpperCase();

    if (keyword !== "") {
        // menu 1
        var voituresLink              = document.getElementById("voitures-link");
        var motosLink                 = document.getElementById("motos-link");
        var caravaningLink            = document.getElementById("caravaning-link");
        var camionsLink               = document.getElementById("camions-link");
        var nautismeLink              = document.getElementById("nautisme-link");
        var utilitairesLink           = document.getElementById("utilitaires-link");
        var equipementsAutoLink       = document.getElementById("equipements-auto-link");
        var equipementsMotosLink      = document.getElementById("equipements-motos-link");
        var equipementsCaravaningLink = document.getElementById("equipements-caravaning-link");
        var equipementsNautismeLink   = document.getElementById("equipements-nautisme-link");






        var immobilierLink = document.getElementById("immobilier-link");
        var loisirsLink = document.getElementById("loisirs-link");
        var allLink = document.getElementById("all-link");




        voituresLink.textContent = keyword + " dans voitures";
        voituresLink.href = "https://ads.capito.tn/search/vehicle-ad?keyword=" + encodeURIComponent(keyword);

        motosLink.textContent = keyword + " dans motos";
        motosLink.href = "https://ads.capito.tn/search/moto-ad?keyword=" + encodeURIComponent(keyword);

        caravaningLink.textContent = keyword + " dans caravaning";
        caravaningLink.href = "https://ads.capito.tn/search/caravaning-ad?keyword=" + encodeURIComponent(keyword);

        camionsLink.textContent = keyword + " dans camions";
        camionsLink.href = "https://ads.capito.tn/search/camions-ad?keyword=" + encodeURIComponent(keyword);

        nautismeLink.textContent = keyword + " dans nautisme";
        nautismeLink.href = "https://ads.capito.tn/search/nautisme-ad?keyword=" + encodeURIComponent(keyword);

        utilitairesLink.textContent = keyword + " dans utilitaires";
        utilitairesLink.href = "https://ads.capito.tn/search/utilitaires-ad?keyword=" + encodeURIComponent(keyword);

        equipementsAutoLink.textContent = keyword + " dans équipements auto";
        equipementsAutoLink.href = "https://ads.capito.tn/search/vehicle-ad?keyword=" + encodeURIComponent(keyword);

        equipementsMotosLink.textContent = keyword + " dans équipements motos";
        equipementsMotosLink.href = "https://ads.capito.tn/search/moto-ad?keyword=" + encodeURIComponent(keyword);

        equipementsCaravaningLink.textContent = keyword + " dans équipements caravaning";
        equipementsCaravaningLink.href = "https://ads.capito.tn/search/vehicle-ad?keyword=" + encodeURIComponent(keyword);

        equipementsNautismeLink.textContent = keyword + " dans équipements nautisme";
        equipementsNautismeLink.href = "https://ads.capito.tn/search/moto-ad?keyword=" + encodeURIComponent(keyword);




        immobilierLink.textContent = keyword + " dans immobilier";
        immobilierLink.href = "https:/ads.capito.tn/immobilier?q=" + encodeURIComponent(keyword);

        loisirsLink.textContent = keyword + " dans loisirs";
        loisirsLink.href = "https:/ads.capito.tn/loisirs?q=" + encodeURIComponent(keyword);

        allLink.textContent = keyword + " dans tout";
        loisirsLink.href = "https:/ads.capito.tn/?q=" + encodeURIComponent(keyword);

        document.getElementById("search-results").style.display = "block";
    } else {
        document.getElementById("search-results").style.display = "none";
    }


}
