function toggleMenu() {
    // Get the element with the class you want to change
    var element = document.querySelector('.site-menu');
    var toggledIcon = document.getElementById("toggled-icon");
    var content = document.getElementById("content-loader")
    if (element.className === "site-menu") {
        element.className += " responsive";
        toggledIcon.innerHTML = "&#215;";
        content.style.display = 'none';
    } else {
        element.className = "site-menu";
        toggledIcon.innerHTML = "&#9776;";
        content.style.display = 'block';
    }
}