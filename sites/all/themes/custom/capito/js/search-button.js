function expandSearch() {
    var searchInput = document.getElementById("search-ad");
    if (searchInput.className === "search-ad hidden") {
        searchInput.classList.remove("hidden");
        searchInput.classList.add("expanded");
    } else {
        searchInput.classList.remove("expanded");
        searchInput.classList.add("hidden");
    }
}
function collapseSearch() {
    var searchInput = document.getElementById("search-ad");
    searchInput.classList.remove("expanded");
    searchInput.classList.add("hidden");
}
function closeSuggestions() {
    document.getElementById("search-results").style.display = "none";
}
