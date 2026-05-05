cat << 'DIFF' > patch.diff
<<<<<<< SEARCH
            <div class="form-check form-switch mt-4">
                <input class="form-check-input" type="checkbox" id="wpa2enabled" %s>
                <label class="form-check-label" for="wpa2enabled">WPA2 Enterprise</label>
            </div>
=======
            <div class="form-check form-switch mt-4">
                <input class="form-check-input" type="checkbox" id="wpa2enabled" aria-controls="wpa2-container" %s>
                <label class="form-check-label" for="wpa2enabled">WPA2 Enterprise</label>
            </div>
>>>>>>> REPLACE
<<<<<<< SEARCH
    <script>$('#wpa2enabled').change(function () {
            if ($(this).prop('checked')) {
                $('#wpa2-container').show();
                $('#wpa2-container input').prop('disabled', false);
            } else {
                $('#wpa2-container').hide();
                $('#wpa2-container input').prop('disabled', true);
            }
        })</script>
=======
    <script>
        function toggleWpa2() {
            var isChecked = $('#wpa2enabled').prop('checked');
            if (isChecked) {
                $('#wpa2-container').show();
                $('#wpa2-container input, #wpa2-container textarea').prop('disabled', false);
            } else {
                $('#wpa2-container').hide();
                $('#wpa2-container input, #wpa2-container textarea').prop('disabled', true);
            }
            $('#wpa2enabled').attr('aria-expanded', isChecked);
        }
        $('#wpa2enabled').change(toggleWpa2);
        $(document).ready(toggleWpa2);
    </script>
>>>>>>> REPLACE
DIFF
