var search_SearchBlock_promptString, search_SearchBlock_input,
    search_SearchBlock_errorString, search_SearchBlock_inProgressString;
var search_submitted = false;

function search_SearchBlock_init(prompt, error, inProgress) {
    search_SearchBlock_promptString = prompt;
    search_SearchBlock_errorString = error;
    search_SearchBlock_inProgressString = inProgress;
    search_SearchBlock_input = document.getElementById('search_SearchBlock').searchCriteria;

    search_SearchBlock_input.value = prompt;
}

function search_SearchBlock_checkForm() {
    var sc = search_SearchBlock_input.value;
    if (search_submitted) {
	alert(search_SearchBlock_inProgressString);
	return false;
    } else if (sc == search_SearchBlock_promptString || sc == '') {
	alert(search_SearchBlock_errorString);
	return false;
    }
    document.getElementById('search_SearchBlock').submit();
    search_submitted = true;
    return true;
}

function search_SearchBlock_focus() {
    if (search_SearchBlock_input.value == search_SearchBlock_promptString) {
	search_SearchBlock_input.value = '';
    }
}

function search_SearchBlock_blur() {
    if (search_SearchBlock_input.value == '') {
	search_SearchBlock_input.value = search_SearchBlock_promptString;
    }
}
