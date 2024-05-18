(function ($) {
    // Function to hide or show company fields based on the selected account type.
    function toggleCompanyFields() {
        var accountType = $('select[name="field_account_type[und]"]').val();
        if (accountType === 'particular') {
            $('#edit-field-name-company, #edit-field-siret-company,#edit-field-desc-company, #edit-field-email-company, #edit-field-phone-company').hide();
        } else {
            $('#edit-field-name-company, #edit-field-siret-company, #edit-field-desc-company, #edit-field-email-company, #edit-field-phone-company').show();
        }
    }
    // Run the function on page load.
    $(document).ready(function () {
        toggleCompanyFields();
        // Bind change event to the account type field.
        $('select[name="field_account_type[und]"]').change(function () {
            toggleCompanyFields();
        });
    });
})(jQuery);

