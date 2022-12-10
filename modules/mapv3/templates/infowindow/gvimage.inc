<?php
//Fix the description , summary and title HREFs
$tmp = array();

foreach ($map['mapPoints'] as $point) {
	$point['description'] = str_replace('[url', "<a target='gpop' onClick=\"window.open(href, 'gpop','left=20,top=20,scrollbars=yes').focus();return false;\" href", $point['description']);
	$point['description'] = str_replace('[/url]', '</a>', $point['description']);
	$point['description'] = str_replace(']', '>', $point['description']);

	$point['title']   = str_replace('[url', "<a target='gpop' onClick=\"window.open(href, 'gpop','left=20,top=20,scrollbars=yes').focus();return false;\" href", $point['title']);
	$point['title']   = str_replace('[/url]', '</a>', $point['title']);
	$point['title']   = str_replace(']', '>', $point['title']);
	$point['summary'] = str_replace('[url', "<a target='gpop' onClick=\"window.open(href, 'gpop','left=20,top=20,scrollbars=yes').focus();return false;\" href", $point['summary']);
	$point['summary'] = str_replace('[/url]', '</a>', $point['summary']);
	$point['summary'] = str_replace(']', '>', $point['summary']);


	if ($point['type'] == 'GalleryMovieItem') {
		$point['itemLink'] = '<div style=\";margin-top:10px;width:250px;height:180px;overflow:auto;\"><center><embed src=\"' . $gurl->generateUrl(
			array(
				'view'   => 'core.DownloadItem',
				'itemId' => $point['id'],
			),
			false
		) . '\" autoplay=\"true\" controller=\"true\" width=\"180\" height=\"180\" bgcolor=\"#FFFFFF\" aspect=\"false\"></embed></center>';
	} else {
		$point['itemLink'] = '<div style=\";margin-top:10px;width:250px;height:150px;overflow:auto;\"><a href=\"' . $gurl->generateUrl(
			array(
				'view'   => 'core.ShowItem',
				'itemId' => $point['id'],
			),
			false
		) . '\" target=\'gpop\' onClick=\"window.open(href, \'gpop\',\'left=20,top=20,scrollbars=yes\').focus();return false;\">' . $point['title'] . '</a><br/><a href=\"' . $gurl->generateUrl(
			array(
				'view'   => 'core.ShowItem',
				'itemId' => $point['id'],
			),
			false
		) . '\" target=\'gpop\' onClick=\"window.open(href, \'gpop\',\'left=20,top=20,scrollbars=yes\').focus();return false;\"><img src=\"' . $point['thumbLink'] . '\" width=\"' . $point['thumbWidth'] . '\" height=\"' . $point['thumbHeight'] . '\" alt=\"' . $point['title'] . '\" style=\"width:100%;height:auto;\"/></a>';
	}

	$tmp[] = $point;
}
$map['mapPoints'] = $tmp;
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
        +'<br/><br/><a href=\"#zoominlink\" onclick=\"javascript:map.setCenter(new google.maps.LatLng('+lat+','+lon+'));map.setZoom('+zoomlevel+'); return false;\">" . $module->translate('Zoom In') . "</a>'";
}
$map['infowindows'][0] = $map['infowindows'][0] . "
        +'</div>'";
