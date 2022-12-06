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
	+'<br/><font color=black><b>" . $module->translate('Date:') . "</b> '+created+'</font>'";

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
        +'<br/><br/><b>Directions To Location:</b><br/>'
        + 'Start address:<form action=\"https://maps.google.com/maps?daddr='+lat+','+lon+'&saddr=\" target=\"gpop\" onsubmit=\"window.open(action+saddr.value, \'gpop\',\'left=20,top=20,scrollbars=yes\').focus();return false;\">'
        + '<input type=\"text\" size=30 maxlength=40 name=\"saddr\" id=\"saddr\" value=\"\" /><br>'
        + '<input type=\"hidden\" name=\"daddr\" value=\"'+lat+','+lon+'\"/>'
        + '<INPUT value=\"Get Directions\" TYPE=\"SUBMIT\"></form>'";

if ($map['showZoomLinks']) {
	$map['infowindows'][0] = $map['infowindows'][0] . "
        +'<br/><br/><a href=\"#zoominlink\" onclick=\"javascript:map.setCenter(new google.maps.LatLng('+lat+','+lon+'));map.setZoom('+zoomlevel+'); return false;\">" . $module->translate('Zoom In') . "</a>'";
}
$map['infowindows'][0] = $map['infowindows'][0] . "
        +'</div>'";
