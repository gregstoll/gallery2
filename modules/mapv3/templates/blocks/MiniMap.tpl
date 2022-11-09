{*
 * $Revision: 1264 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{* Set defaults *}
{if !isset($item)}{assign var=item value=$theme.item}{/if}
{if !isset($mapHeight)}{assign var=mapHeight value=150}{/if}
{if !isset($mapWidth)}{assign var=mapWidth value="100%"}{/if}
{if !isset($mapType)}{assign var=mapType value=3}{/if}
{if !isset($showControls)}{assign var=showControls value=false}{/if}

{g->callback type="mapv3.MiniMap" itemId=$item.id albumMarker=$albumMarker|default:true albumItems=$albumItems|default:2 useParentCoords=$useParentCoords|default:false}
{if !empty($block.mapv3.MiniMap) and $block.mapv3.MiniMap.APIKey neq '' and !(empty($block.mapv3.MiniMap.mapCenter) and empty($block.mapv3.MiniMap.markers))}
    <div class="{$class} clearfix">
        {if $block.mapv3.MiniMap.blockNum == 1}{* Only include Google Maps script once *}
            <script src="//maps.googleapis.com/maps/api/js?file=api&amp;v=3&amp;key={$block.mapv3.MiniMap.APIKey}"
                    type="text/javascript"></script>
        {/if}
        <script type="text/javascript">
            //<![CDATA[
            {* Append the blockNum to the function name to make it unique in the document *}
            function load_map_{$block.mapv3.MiniMap.blockNum}() {ldelim}
                if (true) {ldelim}
                    var map = new google.maps.Map(document.getElementById("minimap-{$block.mapv3.MiniMap.blockNum}"),
                        {ldelim}
                            mapTypeId: {if $mapType eq 1}google.maps.MapTypeId.ROADMAP{elseif $mapType eq 2}google.maps.MapTypeId.SATELLITE{else}google.maps.MapTypeId.HYBRID{/if},
                            center:
                                {if !empty($block.mapv3.MiniMap.mapCenter)}{* already have a valid mapCenter *}
                                    new google.maps.LatLng({$block.mapv3.MiniMap.mapCenter})
                                {else}
                                    new google.maps.LatLng(0, 0) {* Will reset to auto center after adding markers *}
                                {/if},
                            zoom: {$block.mapv3.MiniMap.mapZoom},
                            {if $showControls}
                            disableDefaultUI: false
                            {else}
                            disableDefaultUI: true
                            {/if}
                        {rdelim});

                    {* Set up all the icons for the markers *}
                    var marker_icons = {ldelim}{rdelim};
                    {foreach from=$block.mapv3.MiniMap.markerIcons key=iconName item=icon}
                    marker_icons["{$iconName}"] = {ldelim}
                        icon : "{$icon.imgUrl}",
                        scaledSize : new google.maps.Size({$icon.width}, {$icon.height}),
                        size : new google.maps.Size({$icon.width}, {$icon.height}),
                        anchor : new google.maps.Point({$icon.width}/2, {$icon.height})
                        {rdelim};
                    {/foreach}
                    {* Now create and add the markers to the map *}
                    var markerCoords;
                    {if empty($block.mapv3.MiniMap.mapCenter)}
                    var autoCenterBounds = new google.maps.LatLngBounds();
                    var maxZoom = 0;
                    {/if}
                    {foreach from=$block.mapv3.MiniMap.markers item=marker}
                    markerCoords = new google.maps.LatLng({$marker.GPS});
                    {if empty($block.mapv3.MiniMap.mapCenter)}
                    {if isset($marker.ZoomLevel)}
                    {* Get the maximum zoom to prevent a useless super-zoomed-in view. *}
                    maxZoom = Math.max(maxZoom, {$marker.ZoomLevel});
                    {/if}
                    autoCenterBounds.extend(markerCoords);
                    {/if}
                    new google.maps.Marker(
                        {ldelim}
                            position: markerCoords,
                            clickable: false,
                            map: map,
                            title: "{$marker.title|escape:'javascript'}",
                            icon: marker_icons["{$marker.icon}"].icon
                        {rdelim});
                    {/foreach}
                    {if empty($block.mapv3.MiniMap.mapCenter)}{* Auto center and zoom based on markers to be shown *}
                    map.setCenter(autoCenterBounds.getCenter(), Math.min(map.getBoundsZoomLevel(autoCenterBounds), maxZoom));
                    {/if}

                    {rdelim}
                {rdelim}

            //
            <!-- Weird workaround onLoad hack for IE; Mozilla doesn't need this extra code -->
            function init_map_{$block.mapv3.MiniMap.blockNum}() {ldelim}
                if (arguments.callee.done) return;
                arguments.callee.done = true;
                load_map_{$block.mapv3.MiniMap.blockNum}();
                {rdelim}

            google.maps.event.addDomListener(window, 'load', init_map_{$block.mapv3.MiniMap.blockNum});
            //]]>
        </script>

        <style>
            .gallery-google-map-wrapper {ldelim}
                max-width: {$mapWidth};
                width: auto;
            {rdelim}
            .gallery-google-map-container {ldelim}
                border:1px solid black;
                height: {$mapHeight}px;
                max-height: 100%;
            {rdelim}
        </style>
        <div class="gallery-google-map-wrapper">
            <div class="block-expandable-header">
                <h3>{g->text text="%s Location Map" arg1=$block.mapv3.MiniMap.ItemType}</h3>
            </div>
            <div id="minimap-{$block.mapv3.MiniMap.blockNum}" class="MiniGMap block-expandable-content gallery-google-map-container"></div>
        </div>
    </div>
{/if}