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
$map['Labels'][0]      = 'none';
$map['extramap']       = '';
$map['infowindows'][0] = "''
	+'<div style=\"width:200px\"><center><a href=\"'+itemLink+'\" target=\'gpop\' onClick=\"window.open(href, \'gpop\',\'left=20,top=20,scrollbars=yes\').focus();return false;\">'+title+'</a><br/>'
	+'<a href=\"'+itemLink+'\" target=\'gpop\' onClick=\"window.open(href, \'gpop\',\'left=20,top=20,scrollbars=yes\').focus();return false;\"><img src=\"'+thumbLink+'\" width=\"'+thw+'\" height=\"'+thh+'\" alt=\"'+title+'\" style=\"max-width:200px;max-height:200px;\"/></a>'
	+'<br/><font color=black><b>" . $module->translate('Date:') . "</b> '+created+'</font>'";

if ($map['showItemSummaries']) {
	$map['infowindows'][0] = $map['infowindows'][0] . "
        +(summary!=\"\"?'<br/><br/><div style=\"width:200px\"><font color=black>'+summary+'</font></div>':'')";
}

if ($map['showItemDescriptions']) {
	$map['infowindows'][0] = $map['infowindows'][0] . "
        +(description!=\"\"?'<br/><br/><div style=\"width:200px\"><font color=black>'+description+'</font></div>':'')";
}

$map['infowindows'][0] = $map['infowindows'][0] . "
        +'<br/><font color=black><b>Location:</b><br>'+lat+','+lon+'</font>'";

if ($map['showZoomLinks']) {
	$map['infowindows'][0] = $map['infowindows'][0] . "
        +'<br/><br/><a href=\"#zoominlink\" onclick=\"javascript:map.setCenter(new google.maps.LatLng('+lat+','+lon+'));map.setZoom('+zoomlevel+'); return false;\">" . $module->translate('Zoom In') . "</a>'";
}
$map['infowindows'][0] = $map['infowindows'][0] . "
        +'</div></center>'";
