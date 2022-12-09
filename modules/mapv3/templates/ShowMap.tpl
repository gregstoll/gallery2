{*
 * $Revision: 1253 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if !$mapv3.fullScreen}
<div id="gsContent" class="gcBorder1">
{/if}
    {literal}
    <style>
        .map-grab-wrapper {
            height: 75vh;
        }
        #gsContent {
            width: 85vw;
        }
    </style>
    {/literal}
{if $mapv3.fullScreen neq 2 and $mapv3.fullScreen neq 3}
    {if $mapv3.mode eq "Normal"}
      <h2>{g->text text="Photo Map"}{if isset($mapv3.Filter)}<span class="giWarning"> {g->text text="Filtered on"} {$mapv3.Filter}</span>{/if}</h2>
    {else} {* mode eq "Pick" *}
      <h2>{g->text text="Grab coordinates from Map"}</h2>
    <div class="map-grab-wrapper">
    <div class="col-xs-12 col-sm-12 col-md-9">
    {/if}
    {if isset($mapv3.filterhackingerror)}
    <div class="gbBlock">
       <h2 class="giError">{g->text text="There was a hacking attempt on the filter name, please remove the filter option from the address bar."}
    </div>
    {/if}
{/if}


{if $mapv3.mode eq "Normal"}
    {if $mapv3.useMarkerSet eq "none"}
    <!--
    If there are no markers, don't display a map. Instead, show a link to the admin page for the
    markers to be created
    -->
    <div class="gbBlock">
    {capture name="mapThemeAdminUrl"}
      {g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapSiteAdmin" arg3="mode=Theme"}
    {/capture}
      <h2 class="giError">{g->text text="There are no markers created or a bad marker set is currently selected. Please review settings in the %scontrol panel%s."
       	arg1="<a href=\"`$smarty.capture.mapThemeAdminUrl`\">" arg2="</a>"}
      </h2>
    </div>
    {/if}

    {if isset($mapv3.nogpscoords) and $mapv3.nogpscoords}
    <!-- If there are no items with GPS coordinate, don't display a map and show a message -->
    <div class="gbBlock">
       <h2 class="giError">{g->text text="There are no items with GPS coordinates"}
       <a href='http://codex.gallery2.org/index.php/Gallery2:Modules:UserGuide'> {g->text text="Check the Wiki for more information"}</a></h2>
       </div>
    {/if}

    {if isset($mapv3.noitemperms) and $mapv3.noitemperms}
    <!-- If there are no items with sufficient permissions to be mapped then display a message -->
    <div class="gbBlock">
        <h2 class="giError">{g->text text="There are no items available to be mapped"}<br/><br/>
        <a href='Javascript:history.go(-1);'>Go Back</a></h2>
    </div>
    {/if}

    {if isset($mapv3.noiteminalbum) and $mapv3.noiteminalbum}
    <!-- No item in the selected album, hacking attempt ? -->
    <div class="gbBlock">
        <h2 class="Warning">
          {g->text text="There were no items found in the selected %s, what would you like to do?"
          arg1=$mapv3.Filter}</h2><br/>
        <h2 style="position:relative;left:100px;">1 - {g->text text="%sGo Back%s to the Album and add coordinates to items"
          arg1="<a href='Javascript:history.go(-1);'>" arg2="</a>"}
        </h2>
        {capture name="mapUrl"}
          {g->url arg1="view=mapv3.ShowMap"}
        {/capture}
        <h2 style="position:relative;left:100px;">2 - {g->text text="Show me the %sdefault map%s"
              arg1="<a href=\"`$smarty.capture.mapUrl`\">" arg2="</a>"}
        </h2>
    </div>
    {/if} {* isset($mapv3.noiteminalbum) and $mapv3.noiteminalbum *}
{/if} {* $mapv3.mode eq "Normal" *}
{if !isset($mapv3.googleMapKey) or $mapv3.googleMapKey eq ''}
    <!-- No Google Map Keys were found to suit this install -->
    <div class="gbBlock">
        <h2 class="giError">
        {capture name="mapAdminUrl"}
          {g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapSiteAdmin"}
	    {/capture}
	    {g->text text="You do not have a profile setup for this website to use the Google Map. Review your settings in the %sAdmin Panel%s or %scheck the Wiki%s."
	        arg1="<a href=\"`$smarty.capture.mapAdminUrl`\">" arg2="</a>"
            arg3="<a href=\"http://codex.gallery2.org/Gallery2:Modules:Map:UserGuide\">" arg4="</a>"}
 	</h2><br/><br/>
    </div>
{else}

    <!-- Create the Div where the map will be displayed  -->
    {if $mapv3.mode eq "Pick" or ($mapv3.mode eq "Normal" and (!isset($mapv3.noiteminalbum) or !$mapv3.noiteminalbum) and (!isset($mapv3.nogpscoords) or !$mapv3.nogpscoords) and (!isset($mapv3.noitemperms) or !$mapv3.noitemperms) and isset($mapv3.googleMapKey) and $mapv3.googleMapKey neq '')} {* number 1 *}
        {if $mapv3.mode eq "Pick" or $mapv3.useMarkerSet <> "none"} {* number 2 *}
            {if $mapv3.mode neq "Pick" and isset($mapv3.ShowFilters) and $mapv3.ShowFilters eq "3" and !$mapv3.fullScreen}
                {g->block type="mapv3.mapFilter"}
            {/if}
            {if $mapv3.mode neq "Pick" and (!isset($mapv3.LegendPos) or (isset($mapv3.LegendPos) and $mapv3.LegendPos eq 'top')) and $mapv3.fullScreen neq 3}
                {if $mapv3.mode neq "Pick" and ((isset($mapv3.AlbumLegend)) and ($mapv3.AlbumLegend eq "1")) or ((isset($mapv3.PhotoLegend)) and ($mapv3.PhotoLegend eq "1")) or ((isset($mapv3.regroupItems)) and ($mapv3.regroupItems eq "1")) and $mapv3.fullScreen neq 3}
                    {g->block type="mapv3.Legend"}
                {/if}
            {/if }
            {if $mapv3.mode neq "Pick" and $mapv3.fullScreen neq 3}
                {if $mapv3.ThumbBarPos eq "top" or $mapv3.ThumbBarPos eq "right" or $mapv3.ThumbBarPos eq "left"}
                    {g->block type="mapv3.Thumb"}
                {/if}

                <table align=right style="border-collapse:collapse;">
                    {if isset($mapv3.ShowFilters) and $mapv3.ShowFilters eq "2" and !$mapv3.fullScreen}
                      <tr><td>
                      {g->block type="mapv3.mapFilter"}
                      <br/>
                      </td></tr>
                    {/if}
                    {if $mapv3.mode neq "Pick" and isset($mapv3.LegendPos) and $mapv3.LegendPos eq 'right'}
                      <tr><td>
                       {g->block type="mapv3.Legend"}
                      </td></tr>
                    {/if}
                </table>
            {/if} {* end if $mapv3.mode neq "Pick" and $mapv3.fullScreen neq 3 *}
            <style>
              {literal}
              .map-fullscreen {
                width: 100%;
                height: 100%;
              }
              .map-fluid {
                overflow:hidden;
                width: 100%;
                height: 100%;
                color:black;
                background-color:lightgrey;
                        border: 1px solid black;
              }
              .map-wrapper {
                  height: 65vh;
              }
              {/literal}
          </style>
            <div class="map-wrapper">
                <div id="map" class="themap{if $mapv3.fullScreen eq 3} map-fullscreen{else} map-fluid{/if}">
                    {if $mapv3.mode eq "Normal"}
                    <h3 id="loading">{g->text text="Loading, please wait..."}</h3>
                    {/if}
                 </div> {* End of the map div *}
            </div>
        {if $mapv3.mode eq "Normal" and $mapv3.fullScreen neq 3}
            {if $mapv3.ThumbBarPos eq "bottom"}{g->block type="mapv3.Thumb"}{/if}
            {if isset($mapv3.ShowFilters) and $mapv3.ShowFilters eq "4" and !$mapv3.fullScreen}
              {g->block type="mapv3.mapFilter"}
            {/if}
            {if $mapv3.mode neq "Pick" and (isset($mapv3.LegendPos) and ($mapv3.LegendPos eq 'bottom'))}
                {if ((isset($mapv3.AlbumLegend)) and ($mapv3.AlbumLegend eq "1")) or ((isset($mapv3.PhotoLegend)) and ($mapv3.PhotoLegend eq "1")) or ((isset($mapv3.regroupItems)) and ($mapv3.regroupItems eq "1"))}
                 {g->block type="mapv3.Legend"}
                {/if}
            {/if} {* $mapv3.mode neq "Pick" *}
        {/if} {* $mapv3.mode eq "Normal" and $mapv3.fullScreen neq 3 *}
        {if $mapv3.mode eq "Pick"} {* number 3 *}
            <!--
              Display some very basic help
              -->
            <h2 class="giSuccess">{g->text text="Tips" hint="Hints or suggestions"}</h2>
            <ul>
                <li/>{g->text text="Click on the map to choose the point."}
                <li/>{g->text text="Each click will create a marker to ease aiming."}
                <li/>{g->text text="When you are satisfied with the coordinates, click <B>Save</B> above and the center of the map and the zoom level will be copied in the GPS and ZoomLevel fields of the item."}
            </ul>
            </div>{* col-xs-12 col-md-9 *}
            <div class="col-xs-12 col-sm-12 col-md-3">
                <h2 class="giSuccess">{g->text text="MENU"}</h2>
                <!--
                This creates the URL to save the parameters and return to the right place
                -->
                <form action="{g->url}" method="post" id="itemEdit" enctype="application/x-www-form-urlencoded">
                  <div>
                      {g->hiddenFormVars}
                      <input type="hidden" name="{g->formvar var="controller"}" value="{$controller}"/>
                  </div>
                  <input type="hidden" name="{g->formVar var="form[id]"}" value="{$form.id}"/>
                  <input type="hidden" name="{g->formVar var="form[mode]"}" value="{$mapv3.mode}"/>
                  <input type="hidden" name="{g->formVar var="form[plugin]"}" value="{$mapv3.plugin}"/>
                  <input type="hidden" id="coord" name="{g->formVar var="form[coord]"}" value="{if $mapv3.centerLongLat neq 'none'}{$mapv3.centerLongLat}{else}-12,20{/if}"/>
                  <input type="hidden" id="zoom" name="{g->formVar var="form[zoom]"}" value="{if $mapv3.zoomLevel neq 'none'}{$mapv3.zoomLevel}{else}16{/if}"/>
                            <input type="submit" name="{g->formVar var="form[save]"}" value="{g->text text="Save these coordinates"}"
                                   class="inputTypeSubmit"/>
                            <input type="submit" name="{g->formVar var="form[cancel]"}"
                                   value="{g->text text="Cancel" hint="Discard changes"}" class="inputTypeSubmit"/>
                </form>
                <div class="gbBlock">
                    <span>{g->text text="Coordinates"}:</span>
                    <span id="message_id"><strong>({if $mapv3.centerLongLat neq 'none'}{$mapv3.centerLongLat}{else}-12,20{/if})</strong></span>
                    <br/>
                    <span>{g->text text="Zoom level"}:</span>
                    <span id="zoom_id"><strong>{if $mapv3.zoomLevel neq 'none'}{$mapv3.zoomLevel}{else}16{/if}</strong></span>
                </div>
                <h2>{$theme.item.title|markup}</h2>
                {* TODO: Do not add thumbnail if the item does not have one *}
                        <img src="{if isset($form.itemthumb)}{$form.itemthumb}{/if}" class="giThumbnail"/>
                        <strong>{g->text text="Summary"}</strong>
                        <p>{$theme.item.summary|markup}</p>
                        <strong>{g->text text="Description"}</strong>
                        <p>{$theme.item.description|markup}</p>
                        <strong>{g->text text="Keywords"}</strong>
                        <p>{$theme.item.keywords|markup}</p>
            </div>{* col-xs-12 col-sm-12 col-md-3 *}
    </div> {* map-grab-wrapper *}
                {/if}{* $mapv3.mode eq "Pick" number 3 *}
            {/if} {* $mapv3.mode eq "Pick" number 2 *}
        {/if} {* $mapv3.mode eq "Pick" number 1 *}
    {/if} {*  !isset($mapv3.googleMapKey) or $mapv3.googleMapKey eq '' *}
{if $mapv3.hasadminrights and $mapv3.fullScreen neq 3}
    <div>
<a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapSiteAdmin"}">{g->text text="Google Map Administration"}</a>
    </div>
{/if}
{if !$mapv3.fullScreen}
    </div><!-- #gsContent -->
{/if}

