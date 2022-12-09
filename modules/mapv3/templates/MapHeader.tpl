{*
 * $Revision: 1264 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{* {literal}
<style type="text/css">
a {ldelim}overflow: hidden;{rdelim}
a:hover {ldelim} outline: none; {rdelim}
</style>
{/literal} *}

{assign var=barPosition value="`$mapv3.ThumbBarPos`"}
{if isset($mapv3) }
	{include file="modules/mapv3/includes/GoogleMap.css"}
{/if}
<!-- Google Maps script -->
{if isset($mapv3.googleMapKey) and $mapv3.googleMapKey neq 'f'}
<script src="//maps.googleapis.com/maps/api/js?file=api&amp;v=3&amp;key={$mapv3.googleMapKey}"
            type="text/javascript"></script>

<script src="{g->url href="modules/mapv3/GoogleMap.js"}" type="text/javascript"></script>
<!-- This is mostly boilerplate code from Google. See: http://www.google.com/apis/maps/documentation/ -->

<script type="text/javascript">
    //<![CDATA[

    var DEBUGINFO = 0; //set to 1 to view the Glog, 0 otherwise
    var allInfoWindows = [];

    var lat, lon, itemLink, title, thumbLink, thw, thh, created, summary, zoomlevel;
    var mapConfig = {ldelim}
        {if $mapv3.mode eq "Normal" and isset($mapv3.ThumbBarPos) and $barPosition neq "hidden" and $mapv3.fullScreen neq 3}
        /* initialize some variable for the sidebar */
        'sidebarheight' : {$mapv3.ThumbHeight+4},
        {* Should this be ThumbWidth? *}
        'sidebarwidth' : {$mapv3.ThumbHeight+4},
        {/if}
        {* Text string translations *}

        '_divhistorytext' : '{g->text text="Move history" forJavascript=true}:',
        '_movetext' : '{g->text text="move" forJavascript=true}',
        '_zoomtext' : '{g->text text="zoom" forJavascript=true}',
        '_starttext' : '{g->text text="start" forJavascript=true}',
        '_windowtext' : '{g->text text="window" forJavascript=true}',

        {*                     *}
        {*                     *}
        {*      MAP WIDTH      *}
        {*                     *}
        {*                     *}

        {if $mapv3.mode eq "Normal"}
            {assign var='minusW' value='0'}

            {if $mapv3.sidebar eq 1 and $mapv3.fullScreen eq 0}
                {assign var='minusW' value='210'}
            {else}
                {assign var='minusW' value='20'}
            {/if}
            {if ($mapv3.LegendPos eq 'right' and $mapv3.LegendFeature neq '0' and ($mapv3.AlbumLegend or $mapv3.PhotoLegend or (isset($mapv3.regroupItems) and $mapv3.regroupItems))) or ($mapv3.FilterFeature neq '0' and isset($mapv3.ShowFilters) and $mapv3.ShowFilters eq "right")}
                {assign var='minusW' value="$minusW + 155"}
            {/if}

        {elseif $mapv3.mode eq "Pick"}
            {assign var='minusW' value='410'}
        {/if}

        {if $barPosition eq "right" or $barPosition eq "left"}
            {assign var='minusW' value="$minusW + `$mapv3.ThumbHeight` + 30"}
        {/if}

	'minusW': '{$minusW}',
        {* Calculate the width and weight of the map div, it permits the use of percentages or fixed pixel size *}
        {if $mapv3.WidthFormat eq "%"}
        'myWidth' : getmapwidth({$mapv3.mapWidth},{$minusW});
        {else}
        'myWidth' : {$mapv3.mapWidth},
        {/if}

        {*                     *}
        {*                     *}
        {*      MAP HEIGHT     *}
        {*                     *}
        {*                     *}
        {if $mapv3.mode eq "Normal"}
            {assign var='minusH' value='150'}
            {if $mapv3.fullScreen eq 2}{assign var='minusH' value="$minusH - 120"}{/if}
            {if $mapv3.ShowFilters eq "top" or $mapv3.ShowFilters eq "bottom"}{assign var='minusH' value="$minusH + 25"}{/if}
            {if $mapv3.LegendPos eq 'top' or $mapv3.LegendPos eq 'bottom'}{assign var='minusH' value="$minusH + 90"}{/if}
            {if $barPosition eq "top" or $barPosition eq "bottom"}{assign var='minusH' value="$minusH + `$mapv3.ThumbHeight` + 25"}{/if}
        {elseif $mapv3.mode eq "Pick"}
	    {assign var='minusH' value='155'}
        {/if}

	'minusH': '{$minusH}',
        {if $mapv3.HeightFormat eq "%"}
        'myHeight' : getmapheight({$mapv3.mapHeight},{$minusH});
        {else}
        'myHeight' : {$mapv3.mapHeight},
        {/if}

	'myZoom' : {$mapv3.zoomLevel},
        'ARROW_IMG_URL' : "{g->url href='modules/mapv3/images/arrow.png'}",

	'centerLongLat': "{$mapv3.centerLongLat}",
        'mapType': '{$mapv3.mapType}',

        /* TODO: keep it for a lazy evaluation */
        'htmls' : {strip}{if isset($mapv3.infowindows)}
            [{foreach from=$mapv3.infowindows item=infowindow key=num}{if $num >0},{/if}{$infowindow}{/foreach}]
                {else}
            []
            {/if}{/strip},

        /* TODO: keep it for a lazy evaluation */
        'labels' : {strip}{if isset($mapv3.Labels)}
            [{foreach from=$mapv3.Labels item=Labels key=num}{if $num >0}, {/if}"{$Labels}"{/foreach}]
                {else}
            []
            {/if}{/strip},

        'markerSizeX': {if isset($mapv3.MarkerSizeX)}{$mapv3.MarkerSizeX}{else}0{/if},
        'markerSizeY': {if isset($mapv3.MarkerSizeY)}{$mapv3.MarkerSizeY}{else}0{/if},
        'IMAGE_UP': '{g->url href="modules/mapv3/images/up.png"}',
        'IMAGE_DOWN': '{g->url href="modules/mapv3/images/down.png"}',

    {rdelim};
    {capture assign='IMAGE_UP'}{g->url href='modules/mapv3/images/up.png'}{/capture}
    {capture assign='IMAGE_DOWN'}{g->url href='modules/mapv3/images/down.png'}{/capture}

    {literal}
    // ===== Close all opened info windows =====
    function closeAllInfoWindows(){
        for (var i=0;i<allInfoWindows.length;i++) {
	    allInfoWindows[i].close();
        }
    }

    /*
     *
     * Global functions
     *
     **/

    // ===== Show and Hide markers =====
    function markerDisplay(number,show,type) {
        if (type != 'Regroup'){
	    if ((show) && (!markers[number].onmap)) {
	        if (DEBUGINFO) console.debug('Normal Icon,show,'+number);
	        markers[number].onmap = true;
	        markers[number].setMap(map);
	    }
	    if ((!show) && (markers[number].onmap)) {
	        if (DEBUGINFO) console.debug('Normal Icon,hide,'+number);
	        markers[number].onmap = false;
	        markers[number].setMap(null);
	    }
        }else{
	    if ((show) && (!Rmarkers[number].onmap)) {
	        if (DEBUGINFO) console.debug('Regroup Icon,show,'+number);
	        Rmarkers[number].onmap = true;
	        Rmarkers[number].setMap(map);
	    }
	    if ((!show) && (Rmarkers[number].onmap)) {
	        if (DEBUGINFO) console.debug('Regroup Icon,hide,'+number);
	        Rmarkers[number].onmap = false;
	        Rmarkers[number].setMap(null);
	    }
        }
    }

    /* functions related to the Thumbnail bar */
    function show_arrow(number,xcoord,ycoord,type){
        if (DEBUGINFO) console.debug('Show '+number+','+type);
        if (DEBUGINFO) console.debug('Hiding the Icon');
        markerDisplay(number,0,type);
        var icon = {};
        icon.url = ARROW_IMG_URL;
        icon.size = new google.maps.Size(20, 30);
        icon.anchor = new google.maps.Point(10, 30);
        // icon.infoWindowAnchor = new google.maps.Point(9, 2);
        var point = new google.maps.LatLng(xcoord, ycoord);
        var newarrow = new google.maps.Marker({ position: point, icon: icon.url});
        arrow = newarrow;
        newarrow.setMap(map);
    }

    function hide_arrow(number,type){
        var marker = markers[number];
        if (type != 'normal') marker = Rmarkers[number];
        if (DEBUGINFO) console.debug('hide: '+number+','+type+';myzoom:'+myZoom+'; low:'+marker.showLow+',high:'+marker.showHigh);
        if (myZoom <= marker.showLow && myZoom >= marker.showHigh) {
	    if (DEBUGINFO) console.debug('Showing the Icon');
	    markerDisplay(number,1,type); //marker.display(true);
        }
        arrow.setMap(null);
    }

    function createControlCloseInfoWindows(map){
        var closeControlDiv = document.createElement('div');
        var closeControl = new CloseInfoWindowsControl(closeControlDiv, map);

        closeControlDiv.index = 1;
        map.controls[google.maps.ControlPosition.TOP_RIGHT].push(closeControlDiv);
    }

    function CloseInfoWindowsControl(controlDiv, map) {

        // Set CSS for the control border.
        var controlUI = document.createElement('div');
        controlUI.style.backgroundColor = '#fff';
        controlUI.style.border = '2px solid #fff';
        controlUI.style.borderRadius = '3px';
        controlUI.style.boxShadow = '0 2px 6px rgba(0,0,0,.3)';
        controlUI.style.cursor = 'pointer';
        controlUI.style.margin = '1em';
        controlUI.style.textAlign = 'center';
        controlUI.title = 'Click to close all info windows in the map';
        controlDiv.appendChild(controlUI);

        var controlImage = document.createElement('img');
        controlImage.src = "data:image/svg+xml,%3C%3Fxml version='1.0' encoding='UTF-8'%3F%3E%3Csvg enable-background='new 0 0 32 32' version='1.1' viewBox='0 0 32 32' xml:space='preserve' xmlns='http://www.w3.org/2000/svg'%3E%3Crect width='32' height='32' fill='none'/%3E%3Ccircle cx='16' cy='28' r='4'/%3E%3Cpath d='m23.735 27.666h-2e-3 2e-3zm6.264-11.667c-2e-3 -7.732-6.268-13.999-14-14-7.732 1e-3 -13.999 6.268-14 14 0 3.094 1.015 5.964 2.721 8.281l-2.72 2.72h8v-8l-2.404 2.404c-1.007-1.559-1.595-3.406-1.596-5.405 0.01-5.521 4.479-9.989 10-10 5.521 0.01 9.989 4.479 9.999 10 2e-3 3.483-1.775 6.535-4.479 8.333l2.215 3.333c3.769-2.502 6.264-6.799 6.264-11.666z'/%3E%3C/svg%3E%0A";
        controlImage.style.height = "18px";
        controlImage.style.width = "18px";
        controlImage.style.margin = "9px";
        controlUI.appendChild(controlImage);

        // Setup the click event listeners: simply set the map to Chicago.
        controlUI.addEventListener('click', function() {
	    closeAllInfoWindows();
        });
    }

    function setMapCenter(map, position){
        map.setCenter(position);
    }

    function fromLatLngToPoint(latLng, map) {
        var topRight = map.getProjection().fromLatLngToPoint(map.getBounds().getNorthEast());
        var bottomLeft = map.getProjection().fromLatLngToPoint(map.getBounds().getSouthWest());
        var scale = Math.pow(2, map.getZoom());
        var worldPoint = map.getProjection().fromLatLngToPoint(latLng);
        return new google.maps.Point((worldPoint.x - bottomLeft.x) * scale, (worldPoint.y - topRight.y) * scale);
    }

    function showTooltip(marker) {
        tooltip.innerHTML = marker.tooltip;
        var point = map.getCenter();
        var offset = fromLatLngToPoint(marker.position, map);
        var anchor = marker.anchorPoint;
        var height = tooltip.clientHeight;

        tooltip.style.visibility="visible";
    }

    {/literal}

    {if $mapv3.mode eq "Normal" and isset($mapv3.ThumbBarPos) and $barPosition neq "hidden" and $mapv3.fullScreen neq 3}
    /* initialize some variable for the sidebar */
    var sidebarheight = {$mapv3.ThumbHeight+4};
    {* Should this be ThumbWidth? *}
    var sidebarwidth = {$mapv3.ThumbHeight+4};
    {/if}
    var sidebarhtml = '';
    var sidebarsize = 0;

    //Create the Map variable to be used to store the map infos
    var map;
    //Variable for the google map so that we can get it translated
    var _divhistorytext = '{g->text text="Move history" forJavascript=true}:'
    var _movetext = '{g->text text="move" forJavascript=true}';
    var _zoomtext = '{g->text text="zoom" forJavascript=true}';
    var _starttext = '{g->text text="start" forJavascript=true}';
    var _windowtext = '{g->text text="window" forJavascript=true}';

    {* Calculate the width and weight of the map div, it permits the use of percentages or fixed pixel size *}
    var myWidth = {$mapv3.mapWidth};
    {if $mapv3.mode eq "Normal"}var minusW = {if $mapv3.sidebar eq 1 and $mapv3.fullScreen eq 0}210{else}20{/if}{if ($mapv3.LegendPos eq 'right' and $mapv3.LegendFeature neq '0' and ($mapv3.AlbumLegend or $mapv3.PhotoLegend or (isset($mapv3.regroupItems) and $mapv3.regroupItems))) or ($mapv3.FilterFeature neq '0' and isset($mapv3.ShowFilters) and $mapv3.ShowFilters eq "right")}+155{/if};{/if}
    {if $barPosition eq "right" or $barPosition eq "left"}
      minusW +={$mapv3.ThumbHeight}+30;
    {/if}
    {if $mapv3.mode eq "Pick"} var minusW = 410; {/if}
    {if $mapv3.WidthFormat eq "%"} myWidth = getmapwidth(myWidth,minusW); {/if}

    var myHeight = {$mapv3.mapHeight};

    {if $mapv3.mode eq "Normal"}var minusH = 150{if $mapv3.fullScreen eq 2}-120{/if}{if $mapv3.ShowFilters eq "top" or $mapv3.ShowFilters eq "bottom"}+25{/if}{if $mapv3.LegendPos eq 'top' or $mapv3.LegendPos eq 'bottom'}+90{/if}{if $barPosition eq "top" or $barPosition eq "bottom"}+{$mapv3.ThumbHeight}+25{/if};{/if}
    {if $mapv3.mode eq "Pick"} var minusH = 155; {/if}
    {if $mapv3.HeightFormat eq "%"} myHeight = getmapheight(myHeight,minusH); {/if}

    var myZoom = {$mapv3.zoomLevel};

    var markers = [];
    var Rmarkers = [];
    var arrowmarker;
    var bounds = new google.maps.LatLngBounds();
    var maxZoom = 10; // default to somewhat zoomed-out
    var ARROW_IMG_URL = "{g->url href="modules/mapv3/images/arrow.png"}";
    var zIndex = 0;


    function ShowMeTheMap(){ldelim}

     if (DEBUGINFO) console.debug('Initializing Map');
     var marker_num = 0;
     var Rmarker_num = 0;

    if (DEBUGINFO) console.debug('Create the Map');
    //Google Map implementation
    map = new google.maps.Map(document.getElementById("map"));

    createControlCloseInfoWindows(map);

   // ================= infoOpened LISTENER ===========
    {* todo: InfoWindow listeners
    {literal}
    google.maps.event.addListener(referenceToInfoWindow, 'domready', function(){
        console.log("Pending implementation of listener");
    };

    GEvent.addListener(map, "infowindowopen", function()  {ldelim}
    infoOpened = true;	// set infoOpened to true to change the history text
    {rdelim});
    // ================= infoClosed LISTENER ===========
    GEvent.addListener(map, "infowindowclose", function()  {ldelim}
    infoOpened = false;	// set infoOpened to false
    {rdelim});
    {/literal}
    *}

    //Initialize the zoom and center of the map where it need to be and the Map Type
    if (DEBUGINFO) console.debug('Set the center, zoom and map type');
    if (DEBUGINFO) console.debug("{$mapv3.centerLongLat} "+myZoom+" {$mapv3.mapType}");
    var point = new google.maps.LatLng ({$mapv3.centerLongLat});
    map.setCenter(point);
    map.setZoom(myZoom);
    map.setMapTypeId("{$mapv3.mapType}");

    {if $mapv3.mode eq "Pick"}

    var center_marker = new google.maps.Marker(
        {ldelim}
            position: point,
            clickable: false,
            map: map
        {rdelim});
    {/if}

    if (DEBUGINFO) console.debug('done!');

    if (DEBUGINFO) console.debug('TODO: Creating the tooltip div');
    {* todo: Tooltip {literal}
        See for an example at
        https://developers.google.com/maps/documentation/javascript/examples/overlay-popup
    {/literal}
    *}
    tooltip = document.createElement("div");
    tooltip.id = "map-tooltip";
        {* todo: Tooltip {literal}
        map.getPane(G_MAP_FLOAT_PANE).appendChild(tooltip);
        // Add tooltip to the dom
            {/literal}
        *}
    tooltip.style.visibility="hidden";
    if (DEBUGINFO) console.debug('done!');

    {if $mapv3.mode eq "Normal"}

        var BaseIcon = {ldelim}{rdelim};
        BaseIcon.size = new google.maps.Size({$mapv3.MarkerSizeX},{$mapv3.MarkerSizeY});
        BaseIcon.anchor = new google.maps.Point(6, 20);
        BaseIcon.infoWindowAnchor = new google.maps.Point(5, 1);
        //Create the base for all icons

        var base_icon = {ldelim}{rdelim};
        base_icon.size = new google.maps.Size({$mapv3.MarkerSizeX},{$mapv3.MarkerSizeY});
        base_icon.anchor = new google.maps.Point(6, 20);
        base_icon.infoWindowAnchor = new google.maps.Point(5, 1);
        {* Variables VARIABLES *}

        var default_photo_icon = {ldelim}{rdelim};
        default_photo_icon.url = "{g->url href="modules/mapv3/images/markers/`$mapv3.useMarkerSet`/marker_`$mapv3.defaultphotocolor`.png"}";
        default_photo_icon.size = new google.maps.Size({$mapv3.MarkerSizeX},{$mapv3.MarkerSizeY});
        default_photo_icon.anchor = new google.maps.Point({$mapv3.MarkerSizeX} / 2, {$mapv3.MarkerSizeY});

        var default_album_icon = {ldelim}{rdelim};
        default_album_icon.url = "{g->url href="modules/mapv3/images/markers/`$mapv3.useAlbumMarkerSet`/marker_`$mapv3.defaultalbumcolor`.png"}";
        default_album_icon.size = new google.maps.Size({$mapv3.AlbumMarkerSizeX},{$mapv3.AlbumMarkerSizeY});
        default_album_icon.anchor = new google.maps.Point({$mapv3.AlbumMarkerSizeX} / 2,{$mapv3.AlbumMarkerSizeY});

        var default_group_icon = {ldelim}{rdelim};
        default_group_icon.url = "{g->url href="modules/mapv3/images/markers/`$mapv3.useGroupMarkerSet`/marker_`$mapv3.defaultgroupcolor`.png"}";
        default_group_icon.size = new google.maps.Size({$mapv3.GroupMarkerSizeX},{$mapv3.GroupMarkerSizeY});
        default_group_icon.anchor = new google.maps.Point({$mapv3.GroupMarkerSizeX} / 2,{$mapv3.GroupMarkerSizeY});

    {if (isset($mapv3.regroupItems) and $mapv3.regroupItems)}
            var replaceIcon = {ldelim}{rdelim};
            replaceIcon.size = new google.maps.Size({$mapv3.ReplaceMarkerSizeX},{$mapv3.ReplaceMarkerSizeY});
            replaceIcon.anchor = new google.maps.Point({$mapv3.replaceAnchorPos});
            replaceIcon.url = "{g->url href="modules/mapv3/images/multi/`$mapv3.regroupIcon`.png"}";

            function CreateRegroup(lat, lon, showLow, showHigh, nbDirect, nbItems, nbGroups) {ldelim}
	        var point = new google.maps.LatLng(lat, lon);
	        {if !isset($mapv3.Filter) or (isset($mapv3.Filter) and ($mapv3.Filter|truncate:5:"" neq 'Route'))}bounds.extend(point);{/if}
	        var marker = new google.maps.Marker(point, replaceIcon)
	        marker.onmap = true;
	        marker.showHigh = showHigh;
	        marker.showLow = showLow;
	        GEvent.addListener(marker, "mouseover", function () {ldelim}
	            showTooltip(marker);
	            {rdelim});
	        GEvent.addListener(marker, "mouseout", function () {ldelim}
	            tooltip.style.visibility = "hidden";
	            {rdelim});
	        GEvent.addListener(marker, "click", function () {ldelim}
	            tooltip.style.visibility = "hidden";
	            map.setCenter(point, showLow + 1);
	            {rdelim});
	        var directText = nbDirect > 0 ? "" + nbDirect + " items and " + (nbItems - nbDirect) + " more " : "";
	        var subgroupText = nbGroups > 0 ? " (" + directText + "in " + nbGroups + " subgroups)" : "";
	        var title = '' + nbItems + ' elements here' + subgroupText + '. Click to zoom in.';
	        marker.tooltip = '<div class="tooltip">' + title + '<\/div>';
	        marker.type = 'Regroup';
	        Rmarkers[Rmarker_num] = marker;
	        Rmarker_num++;
	        marker.setMap(map);
	        {rdelim}
        {/if}
        {literal}
        function CreateMarker(lat, lon, itemLink, title, thumbLink, created, zoomlevel, thw, thh, summary, description, icon, showLow, showHigh, hide, type) {
        {/literal}
            var htmls = [{foreach from=$mapv3.infowindows item=infowindow key=num}{if $num >0},{/if}{$infowindow}{/foreach}];
            var labels = [{foreach from=$mapv3.Labels item=Labels key=num}{if $num >0}, {/if}"{$Labels}"{/foreach}];
            var point = new google.maps.LatLng(lat, lon);
            var infowindow = null;

            {if !isset($mapv3.Filter) or (isset($mapv3.Filter) and ($mapv3.Filter|truncate:5:"" neq 'Route'))}
            bounds.extend(point);
            maxZoom = Math.max(maxZoom, zoomlevel);
            {/if}
            {literal}
            var marker = new google.maps.Marker({position: point, icon: icon.url});

            marker.onmap = true;
            marker.showHigh = showHigh;
            marker.showLow = showLow;
            marker.addListener("mouseover", function () {
                showTooltip(marker);
            });
            marker.addListener("mouseout", function () {
                tooltip.style.visibility = "hidden";
            });
            marker.addListener("click", function () {
                tooltip.style.visibility = "hidden";
                zIndex += 1;
                if (infowindow === null) {
                    if (htmls.length > 2) {
	                htmls[0] = '<div style="width:' + htmls.length * 88 + 'px">' + htmls[0] + '<\/div>';
                    }
                    var info_content = htmls.join();
                    infowindow = new google.maps.InfoWindow({content: info_content});
                    allInfoWindows.push(infowindow);
                }
                infowindow.open(map, marker);
                infowindow.setZIndex(zIndex);
                setMapCenter(map, point);
                var thumb = document.querySelector('#thumb'+this.num);
                if (thumb){
                    var thumbPrevious = document.querySelector('.thumbbar .active');
                    if (thumbPrevious) {
                        thumbPrevious.classList.remove('active');
                    }
                    thumb.scrollIntoView({behavior: 'smooth', block: 'end', inline: 'center'});
                    thumb.classList.toggle('active');
                }
            });
            marker.tooltip = '<div class="tooltip">' + title + '<\/div>';
            marker.num = marker_num;
            marker.type = type;
            markers[marker_num] = marker;
            marker_num++;

            marker.setMap(map);

            if (hide == 1) markerDisplay(marker_num - 1, 0, 'normal');
        } /* function CreateMarker */
            {/literal}

        /* Loop over gallery items that have GPS coordinates
            and output code to add them to the map. */
        {if (!empty($mapv3.mapPoints))}
            {counter name="num" start=-1 print=false}
            {counter name="num1" start=-1 print=false}
            {counter name="num2" start=-1 print=false}
            {counter name="num3" start=-1 print=false}
            {counter name="num4" start=-1 print=false}
            /* creates the Thumbnail bar as we go */
            {foreach from=$mapv3.mapPoints item=point}
                  {if $barPosition neq "hidden" and $mapv3.fullScreen neq 3}
                  {* //map.setCenter(new google.maps.Point({$point.gps})); *}
                  sidebarhtml += '' +
                      '<a id="thumb{counter name="num3"}" ' +
                      'href="#" ' +
                      'onclick="new google.maps.event.trigger(markers[{counter name="num2"}], \'click\' ); return false;" ' +
                      'onmouseover="show_arrow({counter name="num"},{$point.gps},\'normal\');" ' +
                      'onmouseout="hide_arrow({counter name="num1"},\'normal\');">' +
                      '<img style="\
                        {strip}{if $barPosition eq "right" or $barPosition eq "left"}width{else}height
                        {/if}:{$mapv3.ThumbHeight}px;"{/strip}' +
                    'src="{$point.thumbLink}"/>{if $barPosition eq "right" or $barPosition eq "left"}<br/>{/if}<\/a>';
                  sidebarsize +={if $barPosition eq "right" or $barPosition eq "left"}{$point.thumbbarHeight}{else}{$point.thumbbarWidth}{/if}+2;
                  {/if}

                  {* Check point type to assign markers and colors *}
                  {if $point.type eq "GalleryAlbumItem"}
                   {assign var=itemType value="album"}
                   {assign var=markerSet value="`$mapv3.useAlbumMarkerSet`"}
                   {assign var=markerColor value="`$mapv3.defaultalbumcolor`"}
                  {elseif $point.type eq "GoogleMapGroup"}
                   {assign var=itemType value="group"}
                   {assign var=markerSet value="`$mapv3.useGroupMarkerSet`"}
                   {assign var=markerColor value="`$mapv3.defaultgroupcolor`"}
                  {else}
                   {assign var=itemType value="photo"}
                   {assign var=markerSet value="`$mapv3.useMarkerSet`"}
                   {assign var=markerColor value="`$mapv3.defaultphotocolor`"}
                  {/if}

                  {assign var=iconDef value="default_"}
                  {if $point.color neq "default"}
                  var {$itemType}_icon = JSON.parse(JSON.stringify(default_{$itemType}_icon));
                  {assign var=iconDef value=""}{* Clear the "Default" and flag that we declared the variable *}
                  {assign var=markerColor value="`$point.color`"}
                  {$itemType}_icon.url = "{g->url href="modules/mapv3/images/markers/`$markerSet`/marker_`$point.color`.png"}";
                  {/if}
                  {* quick hacky fix for missing numbered markers *}
                  {if $mapv3.EnableRouteNumber}
                  {foreach from=$mapv3.routeitem key=name item=items}
                   {foreach from=$items item=id key=num}
                    {if $point.id == $id}
                     {if $iconDef eq "default_"}{* variable hasn't been declared yet *}
                      {assign var=iconDef value=""}{* Clear the "Default" text *}
                      var {$itemType}_icon = JSON.parse(JSON.stringify(default_{$itemType}_icon));
                     {/if}
                     {$itemType}_icon.url = "{g->url href="modules/mapv3/images/routes/`$name`/`$num+1`-marker_`$markerColor`.png"}";
                    {/if}
                   {/foreach}
                  {/foreach}
                  {/if}
                  {if $point.id|truncate:1:"" neq 'T'}
                    {strip}
                    CreateMarker({$point.gps},
                        "{$point.itemLink}",
                        "{$point.title|markup|escape:"javascript"}",
                        "{$point.thumbLink}",
                        "{$point.created}",
                        {$point.zoomlevel},
                        {$point.thumbWidth},
                        {$point.thumbHeight},
                        {if $mapv3.showItemSummaries && !empty($point.summary)}
                            "{$point.summary|markup|escape:"javascript"}"
                        {else}
                            ""
                        {/if},
                        {if $mapv3.showItemDescriptions && !empty($point.description)}
                            "{$point.description|markup|escape:"javascript"}"
                        {else}
                            ""
                        {/if},
                      {$iconDef}{$itemType}_icon,
                      {$point.regroupShowLow},
                      {$point.regroupShowHigh},
                      0,
                      "{$point.type}");
                    {/strip}
                  {/if}
            {/foreach}

            {if isset($barPosition) and $barPosition neq "hidden" and $mapv3.fullScreen neq 3}
                var thumbdiv = document.getElementById("thumbs");
                thumbdiv.innerHTML = sidebarhtml;
                var mapdiv = document.getElementById("map");

                {if $barPosition eq "top" or $barPosition eq "bottom"}
                if (sidebarsize+1 > myWidth)  {ldelim}
                    sidebarheight = {$mapv3.ThumbHeight+25};
                {rdelim}
                thumbdiv.style.height = sidebarheight+"px";
                {else}
                {* Thumbs are squared and so there is only a ThumbHeight but no ThumbWidth *}
                if (sidebarsize+1 > myHeight)  {ldelim}
                    sidebarwidth = {$mapv3.ThumbHeight+25};
                {rdelim}
                thumbdiv.style.width = sidebarwidth+"px";
                {/if}

            {/if}

        {/if}

        /* Loop over routes if any and display them */
        {if (!empty($mapv3.Routes))}
            var point;
            {foreach from=$mapv3.Routes item=routes}
              var points = [];
              {if $routes.5 eq "Yes"}
              {foreach from=$routes[7] item=point}
                 point = new google.maps.LatLng({$point[0]},{$point[1]});
                 points.push(point);
                 {if (isset($mapv3.Filter) and (($mapv3.Filter|truncate:5:"" eq 'Route')))}bounds.extend(point);{/if}
              {/foreach}
              var poly = new google.maps.Polyline({ldelim}
                  path: points,
                  strokeColor: "{$routes[2]}",
                  strokeWeight: {$routes[3]},
                  strokeOpacity: {$routes[4]}
              {rdelim});
              poly.setMap(map);
              {/if}
            {/foreach}
        {/if}

        {if $mapv3.AutoCenterZoom and (!isset($mapv3.Filter) or (isset($mapv3.Filter) and (($mapv3.Filter|truncate:5:"" eq 'Route') or ($mapv3.Filter|truncate:5:"" eq 'Album') or ($mapv3.Filter|truncate:5:"" eq 'Group'))))}
            map.fitBounds(bounds);
        {/if}

        {* set the correct zoom slide notch and show/hide the regrouped item *}
        zoom = map.getZoom();
        myZoom = 19-zoom;
	{literal}

        /* Hide markers outside of showHigh and showLow zoom levels */
        for (var i=0; i < markers.length; i++) { //Updating the normal items
            var marker = markers[i];
            if (zoom <= marker.showLow && zoom >= marker.showHigh) {
              markerDisplay(i,1,'normal'); //marker.display(true);
              var CorrectA = document.getElementById('thumb'+i);
              if (CorrectA != null ) CorrectA.style.display = "inline";
            }
            else {
              markerDisplay(i,0,'normal'); //marker.display(false);
              var CorrectA = document.getElementById('thumb'+i);
              if (CorrectA != null) CorrectA.style.display = "none";
            }
        }

        /* Hide route markers outside of showHigh and showLow zoom levels */
        for (var i=0; i < Rmarkers.length; i++) { //Updating the normal items
            var marker = Rmarkers[i];
            if (zoom <= marker.showLow && zoom >= marker.showHigh) {
              markerDisplay(i,1,'Regroup'); //marker.display(true);
            }
            else {
              markerDisplay(i,0,'Regroup'); //marker.display(false);
            }
        }
        {/literal}
    {elseif $mapv3.mode eq "Pick"}
        {literal}
        map.addListener('center_changed', function () {
	    var center = map.getCenter();
	    center_marker.setPosition(center);
	    var latLngStr = center.toUrlValue(6);
	    document.getElementById("message_id").innerHTML = '(' + latLngStr + ')';
	    document.getElementById("coord").value = latLngStr;
        });

        map.addListener('click', function (event) {
	    var point = event.latLng;
	    var latitude = point.lat();
	    var longitude = point.lng();
	    console.log(latitude + ', ' + longitude);

	    if (point) {
	        center_marker.position = point;
	        map.panTo(point);
	        var latLngStr = point.toUrlValue(6);
	        document.getElementById("message_id").innerHTML = '(' + latLngStr + ')';
	        document.getElementById("coord").value = latLngStr;
            }
        });

        map.addListener('zoom_changed', function (event) {
	    var oldZoomLevel = map.zoom;
	    var newZoomLevel = map.getZoom();

	    var currentZoomText = '' + newZoomLevel;
	    document.getElementById("zoom_id").innerHTML = currentZoomText;
        });
        {/literal}
    {/if}
    {rdelim} /* end ShowMeTheMap() */

    {if $mapv3.mode eq "Normal" and $mapv3.fullScreen neq 3}
    {literal}
    function togglealbumlegend() {
        var displaystyle;
        if (document.getElementById) { // standard
	    displaystyle = document.getElementById("albumlegend").style.display;
	    document.getElementById("albumlegend").style.display = (displaystyle == "none" ? "block" : "none");
        } else if (document.all) { // old msie versions
	    displaystyle = document.all["albumlegend"].style.display;
	    document.all["albumlegend"].style.display = (displaystyle == "none" ? "block" : "none");
        } else if (document.layers) { // nn4
	    displaystyle = document.layers["albumlegend"].style.display;
	    document.layers["albumlegend"].style.display = (displaystyle == "none" ? "block" : "none");
        }
        var imgsrc = document.albumarrow.id;
        document.albumarrow.src = (imgsrc == "down" ? mapConfig.IMAGE_UP : mapConfig.IMAGE_DOWN);
        document.albumarrow.id = (imgsrc == "down" ? "up" : "down");
    }

    function togglephotolegend() {
        if (document.getElementById) { // standard
	    var displaystyle = document.getElementById("photolegend").style.display;
	    document.getElementById("photolegend").style.display = (displaystyle == "none" ? "block" : "none");
        } else if (document.all) { // old msie versions
	    var displaystyle = document.all["photolegend"].style.display;
	    document.all["photolegend"].style.display = (displaystyle == "none" ? "block" : "none");
        } else if (document.layers) { // nn4
	    var displaystyle = document.layers["photolegend"].style.display;
	    document.layers["photolegend"].style.display = (displaystyle == "none" ? "block" : "none");
        }
        var imgsrc = document.photoarrow.id;
        document.photoarrow.src = (imgsrc == "down" ? mapConfig.IMAGE_UP : mapConfig.IMAGE_DOWN);
        document.photoarrow.id = (imgsrc == "down" ? "up" : "down");
    }

    function strLeft(kstr, kchar) {
        var retVal = "-1";

        if (kstr.indexOf(kchar) > -1){
	    retVal = kstr.substring(0, kstr.indexOf(kchar));
        }
        return (retVal);
    }

    function strRight(kstr, kchar) {
        var retVal = "-1";

        if (kstr.indexOf(kchar) > -1){
	    retVal = kstr.substring(kstr.indexOf(kchar) + kchar.length, kstr.length)
        };
        return (retVal);
    }

    function togglemarkers(number) {
        var thetype;
        if (DEBUGINFO) console.debug('Entering Toggle Marker');
        var markercolor;
        var thetd = document.getElementById(number);
        var Itype = number.substring(0, 1);
        if (Itype == "A") thetype = "GalleryAlbumItem";
        else thetype = "GalleryPhotoItem";
        var thecheckbox = document.getElementsByName("C" + number);
        var clickedcolor = strRight(strLeft(thetd.innerHTML, ".png"), "marker_");
        var zoom = map.getZoom();
        if (DEBUGINFO) {
	    console.debug(clickedcolor + ' ' + thetype + ' ' + zoom);
	    if (thecheckbox.item(0).checked) console.debug('Showing');
	    else console.debug('Hiding');
        }
        for (var i = 0; i < markers.length; i++) {
	    var checktype = (markers[i]["type"] == "GalleryAlbumItem") ? "GalleryAlbumItem" : "GalleryPhotoItem";
	    markercolor = strRight(strLeft(markers[i].getIcon(), ".png"), "marker_");
	    if (DEBUGINFO) console.debug('Marker: ' + markers[i]["type"] + ' ' + markercolor + ' ' + markers[i].showLow + ' ' + markers[i].showHigh);
	    if (markercolor == clickedcolor && (checktype == thetype || markers[i]['type'] == "Regroup")) {
	        if (thecheckbox.item(0).checked && zoom <= markers[i].showLow && zoom >= markers[i].showHigh) markerDisplay(i, 1, 'normal'); //markers[i].display(true);
	        else {
		    if (DEBUGINFO) console.debug('Hiding');
		    markerDisplay(i, 0, 'normal'); //markers[i].display(false);
	        }
	    }
        }
    }
    {/literal}
    {/if}

    google.maps.event.addDomListener(window, 'load', ShowMeTheMap);

    var GoogleMap = true;

    //]]>
</script>
{/if}
