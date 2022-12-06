<?php
//Fix the description , summary and title HREFs
$tmp = array();

foreach ($map['mapPoints'] as $point) {
	$point['description'] = str_replace('[url', "<a target='gpop' onClick=\"window.open(href, 'gpop','left=20,top=20,scrollbars=yes').focus();return false;\" href", $point['description']);
	$point['description'] = str_replace('[/url]', '</a>', $point['description']);
	$point['description'] = str_replace(']', '>', $point['description']);
	$point['title']       = str_replace('[url', "<a target='gpop' onClick=\"window.open(href, 'gpop','left=20,top=20,scrollbars=yes').focus();return false;\" href", $point['title']);
	$point['title']       = str_replace('[/url]', '</a>', $point['title']);
	$point['title']       = str_replace(']', '>', $point['title']);
	$point['summary']     = str_replace('[url', "<a target='gpop' onClick=\"window.open(href, 'gpop','left=20,top=20,scrollbars=yes').focus();return false;\" href", $point['summary']);
	$point['summary']     = str_replace('[/url]', '</a>', $point['summary']);
	$point['summary']     = str_replace(']', '>', $point['summary']);
	$tmp[]                = $point;
}
$map['mapPoints'] = $tmp;
//Create the HTML for the window
$map['Labels'][0] = 'none';
$map['extramap']  = '';

$map['infowindows'][0] = "''
	+'<div style=\";margin-top:10px;width:250px;height:150px;overflow:auto;\">'
	+'<a href=\"'+itemLink+'\" target=\'gpop\' onClick=\"window.open(href, \'gpop\',\'left=20,top=20,scrollbars=yes\').focus();return false;\">'+title+'</a><br/>'
	+'<a href=\"'+itemLink+'\" target=\'gpop\' onClick=\"window.open(href, \'gpop\',\'left=20,top=20,scrollbars=yes\').focus();return false;\"><img src=\"'+thumbLink+'\" width=\"'+thw+'\" height=\"'+thh+'\" alt=\"'+title+'\" style=\"max-width:200px;max-height:200px;\"/></a>'
	+'<br/><font color=black><b>" . $module->translate('Date:') . ":</b> '+created+'</font>'";

if ($map['showItemSummaries']) {
	$map['infowindows'][0] = $map['infowindows'][0] . "
        +(summary!=\"\"?'<br/><br/><font color=black><b>Summary:</b><br/>'+summary+'</font>':'')";
}

if ($map['showItemDescriptions']) {
	$map['infowindows'][0] = $map['infowindows'][0] . "
        +(description!=\"\"?'<br/><br/><font color=black><b>Description:</b><br/>'+description+'</font>':'')";
}

$map['infowindows'][0] = $map['infowindows'][0] . "
        +'<br/><br/><font color=black><b>Location:</b><br>'+lat+','+lon+'</font>'";

$map['infowindows'][0] = $map['infowindows'][0] . "
        +'<br/><a href=\"http://terraserver.microsoft.com/image.aspx?t=1&s=10&lon='+lon+'&lat='+lat+'&w=1\" target=\'gpop\''
        +' onClick=\"window.open(href, \'gpop\',\'left=20,top=20,scrollbars=yes\').focus();return false;\">'
        +'View location at Terraserver</a><br/>(Unfortunately, USA only)'";

$map['infowindows'][0] = $map['infowindows'][0] . "
        +'<br/><a href=\"http://topozone.com/map.asp?lat='+lat+'&lon='+lon+'&s=50&size=m&datum=nad83&layer=DRG25\" target=\'gpop\' onClick=\"window.open(href, \'gpop\',\'left=20,top=20,scrollbars=yes\').focus();return false;\">View Localtion at TopoZone</a><br/>(Unfortunately, USA only)'";

$map['infowindows'][0] = $map['infowindows'][0] . "
        +'<br/><a href=\"http://www.weatherbonk.com/weather/index.jsp?query='+lat+','+lon+'\" target=\'gpop\' onClick=\"window.open(href, \'gpop\',\'left=20,top=20,scrollbars=yes\').focus();return false;\">WeatherBonk.com this location</a>'";

if ($map['showZoomLinks']) {
	$map['infowindows'][0] = $map['infowindows'][0] . "
        +'<br/><br/><a href=\"#zoominlink\" onclick=\"javascript:map.setCenter(new google.maps.LatLng('+lat+','+lon+'));map.setZoom('+zoomlevel+'); return false;\">" . $module->translate('Zoom In') . "</a>'";
}
$map['infowindows'][0] = $map['infowindows'][0] . "
        +'</div>'";
