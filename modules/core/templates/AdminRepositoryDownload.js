function showLanguagePacks(source) {
    for (var i in allSources) {
	var el = document.getElementById(allSources[i] + '_languagePacks');
	el.style.display = (allSources[i] == source)
	    ? 'block'
	    : 'none';
    }
    var el = document.getElementById('languageListPlaceholder');
    if (el) {
	el.style.display = 'none';
    }
}
