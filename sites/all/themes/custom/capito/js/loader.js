window.addEventListener("load", function () {
    var loader = document.getElementById("loader");
    var content = document.getElementById("content-loader");
    setTimeout(function () {
        loader.style.display = "none";
    }, 1000);
    content.style.display = "block";
});