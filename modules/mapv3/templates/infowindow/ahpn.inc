<?php
//Fix the description , summary and title HREFs
//Create the HTML for the window
$map['Labels'][0] = 'none';
$map['extramap']  = '';

$map['infowindows'][0] = "''
	+' '+itemLink+' '
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

if ($map['showZoomLinks']) {
	$map['infowindows'][0] = $map['infowindows'][0] . "
        +'<br/><br/><a href=\"#zoominlink\" onclick=\"javascript:map.setCenter(new google.maps.LatLng('+lat+','+lon+'), '+zoomlevel+'); return false;\">" . $module->translate('Zoom In') . "</a>'";
}
$map['infowindows'][0] = $map['infowindows'][0] . "
        +'</div>'";



$map['infowindows'][0] = "''
	+' '+itemLink+' '
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

if ($map['showZoomLinks']) {
	$map['infowindows'][0] = $map['infowindows'][0] . "
        +'<br/><br/><a href=\"#zoominlink\" onclick=\"javascript:map.setCenter(new google.maps.LatLng('+lat+','+lon+'));map.setZoom('+zoomlevel+'); return false;\">" . $module->translate('Zoom In') . "</a>'";
}
$map['infowindows'][0] = $map['infowindows'][0] . "
        +'</div>'";
