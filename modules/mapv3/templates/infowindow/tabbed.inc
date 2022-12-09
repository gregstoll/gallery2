<?php
$map['Labels'][0]      = 'Picture';
$map['Labels'][1]      = 'Details';
$map['extramap']       = '';
$map['infowindows'][0] = "''
      +'<a href=\"'+itemLink+'\">'+title+'</a><br/>'
      +'<a href=\"'+itemLink+'\"><img src=\"'+thumbLink+'\" width=\"'+thw+'\" height=\"'+thh+'\" alt=\"'+title+'\" style=\"max-width:200px;max-height:200px;\"/></a>'";
$map['infowindows'][1] = "''
      +'<table width=200px><tr><td><a href=\"'+itemLink+'\">'+title+'</a><br/>'
      +'<font color=black><b>" . $module->translate('Date:') . "</b> '+created+'</font>'";

if ($map['showItemSummaries']) {
	$map['infowindows'][1] = $map['infowindows'][1] . "
        +(summary!=\"\"?'<br/><br/><div style=\"width:200px\"><font color=black>'+summary+'</font></div>':'')";
}

if ($map['showItemDescriptions']) {
	$map['infowindows'][1] = $map['infowindows'][1] . "
        +(description!=\"\"?'<br/><br/><div style=\"width:200px\"><font color=black>'+description+'</font></div>':'')";
}

if ($map['showZoomLinks']) {
	$map['infowindows'][1] = $map['infowindows'][1] . "
        +'<br/><br/><a href=\"#zoominlink\" onclick=\"javascript:map.setCenter(new google.maps.LatLng('+lat+','+lon+'));map.setZoom('+zoomlevel+'); return false;\">" . $module->translate('Zoom In') . "</a>'";
}

$map['infowindows'][1] = $map['infowindows'][1] . "
        +'</td></tr></table>'";
