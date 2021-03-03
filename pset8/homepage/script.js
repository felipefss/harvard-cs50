(function() {
    $(document).ready(function() {
        // Load home page when DOM is ready
        $('#mainContent').load('home.html');

        // Listen for when a menu button is clicked
        $('#menu button').click(({ target }) => {
            $('.nav-link.active').removeClass('active');
            $(target).addClass('active');

            // Change main content to the desired page
            $('#mainContent').load(`${target.id}.html`);
        });
    });
})();