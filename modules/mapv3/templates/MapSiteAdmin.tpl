{*
 * $Revision: 1576 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
    <h2>{g->text text="Map Module Settings"}{if !empty($form.arrayMapKeys)}<a style="position:relative;left:50px;border-top:2px solid #cecece; border-left:2px solid #cecece; border-bottom:2px solid #4a4a4a;border-right:2px solid #4a4a4a;padding:.2em;padding-right:1em;padding-left:1em;text-decoration:none;background-color:#ebebeb;color:#000;font-weight:normal;font-size:12px;" href="{g->url arg1="view=mapv3.ShowMap"}">{g->text text="Show Google Map"}</a>{/if}</h2>
</div>

<!--
Beginning of error/success displaying
-->
{if isset($status.saved)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="Settings have been saved!"}</h2>
    </div>
{/if}
{if !empty($form.error)}
<div class="gbBlock">
   <h2 class="giError">{g->text text="There was a problem processing your request."}</h2>
</div>
{/if}
{if isset($status.profileDeleted)}
    <div class="gbBlock">
  {if ($status.profileDeleted)}
        <h2 class="giSuccess">{g->text text="Profile successfully deleted."}</h2>
  {else}
	<h2 class="giWarning">{g->text text="Profile was already deleted."}</h2>
    </div>
  {/if}
{/if}
{if isset($status.profilesaved)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="Profile successfully saved."}</h2>
    </div>
{/if}


<!--
End of error/success displaying
-->

{if !empty($form.arrayMapKeys)}{include file="modules/mapv3/templates/MapAdminTab.tpl" mode="General"}{/if}

<!-- Help Div -->
<div id="helpdiv" style="visibility:hidden;border:2px solid black;position:absolute;right:350px;top:200px;width:250px;height:150px;">
    <div id="helphead" style="width:250px;height:25px;background:#150095;color:white;border-bottom:2px solid black;"><img alt="none" {if !$form.IE}style="float:left;" src="{$form.picbase}help.png"{else}src='{$form.picbase}blank.gif' style='float:left;filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");'{/if}/><table style="height:100%;border-collapse:collapse;"><tr><td style="width:230px;vertical-align:middle;"><b>Help Window</b></td><td style="width:25px;cursor:pointer;" onclick="javascript:hidehelp()"><center><b>X</b></center></td></tr></table></div>
    <div id="helptext" style="width:250px;height:123px;color:black;font-size:12px;background:#B2E4F6;-moz-opacity:.75;filter:alpha(opacity=75);opacity:.75;overflow:auto;clear:both;">
    </div>
</div>
<!-- End of Help Div -->

<!--
Displaying the content of the admin panel depending on the "mode"
General Settings
-->
{if $form.mode neq 'mapKey' and !empty($form.arrayMapKeys)}
<div class="gbBlock">
<h3> {g->text text="Map Settings"} </h3>

  <p class="giDescription">
    {g->text text="Select Map settings for the Google API"}
  </p>

    <table class="gbDataTable">
        <tr>
            <th><label for="mapKeys">{g->text text="Google Map Key Profiles"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_KeyProfile,180)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <table border=0 style="border-collapse:collapse;">
                <tr><td rowspan=3>
                <select size="6" id="mapKeys" name="{g->formVar var="form[mapKeys]"}">
                {foreach from=$form.arrayMapKeys item=profile}
                  <option value="{$profile.url}">{$profile.name}</option>
                {/foreach}
                </select>
                </td><td>
                <input style="width:7em;" type="submit" name="{g->formVar var="form[action][addKey]"}" value="{g->text text="Add Profile"}"/>
                </td></tr><tr><td>
                <input style="width:7em;" type="submit" name="{g->formVar var="form[action][editKey]"}" value="{g->text text="Edit Profile"}"/>
                </td></tr><tr><td>
                <input style="width:7em;" type="submit" name="{g->formVar var="form[action][delKey]"}" value="{g->text text="Delete Profile"}"/>
                </td></tr></table>
            </td>
        </tr>
        <tr>
            <th><label for="MapWidth">{g->text text="Map <u>W</u>idth"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_MapWidth,270)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input accesskey="w" type="text" id="MapWidth"
                    name="{g->formVar var="form[mapWidth]"}"
                    value="{$form.mapWidth}" maxlength="4" size="6"/>

                <select id="MapWidthFormat" name="{g->formVar var="form[MapWidthFormat]"}">
                 <option value="1">px</option>
                 <option value="2" {if $form.MapWidthFormat eq "2"}selected=1{/if}>%</option>
                </select>
                {if isset($form.error.mapWidth)}
                    <span class="giError">
                        {g->text text="Map width must be an integer."}
                    </span>
                {/if}
            </td>
        </tr>
        <tr>
            <th><label for="MapHeight">{g->text text="Map <u>H</u>eight"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_MapHeight,270)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input accesskey="h" type="text" id="MapHeight"
                    name="{g->formVar var="form[mapHeight]"}"
                    value="{$form.mapHeight}" maxlength="4" size="6"/>

                <select id="HeightFormat" name="{g->formVar var="form[MapHeightFormat]"}">
                 <option value="1">px</option>
                 <option value="2" {if $form.MapHeightFormat eq "2"}selected{/if}>%</option>
                </select>
                {if isset($form.error.mapHeight)}
                    <span class="giError">
                        {g->text text="Map height must be an integer."}
                    </span>
                {/if}
            </td>
        </tr>
        <tr>
            <th><label for="MapType">{g->text text="Map type" hint="Style of Google map"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_MapType,320)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input accesskey="m" type="radio" id="MapTypeMap"
                    name="{g->formVar var="form[mapType]"}" value="roadmap"
                    {if $form.mapType eq "roadmap"}checked="checked"{/if}/>
                <label for="MapTypeMap">{g->text text="<u>M</u>ap" hint="Map style"}</label>

                <input accesskey="a" type="radio" id="MapTypeSatellite"
                    name="{g->formVar var="form[mapType]"}" value="satellite"
                    {if $form.mapType eq "satellite"}checked="checked"{/if}/>
                <label for="MapTypeSatellite">{g->text text="S<u>a</u>tellite" hint="Map style"}</label>

                <input accesskey="y" type="radio" id="MapTypeHybrid"
                    name="{g->formVar var="form[mapType]"}" value="hybrid"
                    {if $form.mapType eq "hybrid"}checked="checked"{/if}/>
                <label for="MapTypeHybrid">{g->text text="H<u>y</u>brid" hint="Map style"}</label>
            </td>
        </tr>
        <tr>
            <th><label for="showScale">{g->text text="Show Scale" hint="Map zoom scale"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_ShowScale,350)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="checkbox" id="showScale"
                    name="{g->formVar var="form[showScale]"}"
                    {if $form.showScale}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="CenterLongLat">{g->text text="C<u>e</u>nter On"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_MapCenter,400)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input accesskey="e" type="text" id="CenterLongLat"
                    name="{g->formVar var="form[centerLongLat]"}"
                    value="{$form.centerLongLat}" maxlength="30" size="30"
                    {if ((isset($form.AutoCenterZoom)) and ($form.AutoCenterZoom))}disabled{/if}
                    />
                {g->text text="Format: latitude,longitude"}
                <input type="submit" value="{g->text text="Get via a Map"}" name="{g->formVar var="form[action][getviamap]"}"/>
                {if isset($form.error.centerLongLat)}
                    <span class="giError">
                        {g->text text="Please make sure you have formatted the coordinates correctly"}
                    </span>
                {/if}
            </td>
        </tr>
        <tr>
            <th><label for="ZoomLevel">{g->text text="<u>Z</u>oom Level"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_MapZoom,400)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input accesskey="z" type="text" id="ZoomLevel"
                    name="{g->formVar var="form[zoomLevel]"}"
                    value="{$form.zoomLevel}" maxlength="2" size="2"
                    {if ((isset($form.AutoCenterZoom)) and ($form.AutoCenterZoom))}disabled{/if}
                    />
                {g->text text="Please enter a zoom value between 0 and 19"}
                {if isset($form.error.zoomLevel)}
                    <span class="giError">
                        {g->text text="Please specify an integer between 0 to 19"}
                    </span>
                {/if}
            </td>
        </tr>
        <tr>
            <th><label for="AutoCenterZoom">{g->text text="Aut<u>o</u> Center and Zoom"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_AutoCZ,400)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input accesskey="o" type="checkbox" id="AutoCenterZoom"
                    name="{g->formVar var="form[AutoCenterZoom]"}"
                    {if $form.AutoCenterZoom}checked="checked"{/if}
                    value="1" OnClick="ToggleInputs(['{g->formVar var="form[centerLongLat]"}','{g->formVar var="form[zoomLevel]"}'])"/>
                    {g->text text="This will ignore the two settings above."}
            </td>
        </tr>
        <tr>
            <th><label for="showGE">{g->text text="Show the <u>V</u>iew in Google Earth link"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_ShowGE,500)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input accesskey="v" type="checkbox" id="showGE"
                    name="{g->formVar var="form[ShowExportGELink]"}"
                    {if $form.ShowExportGELink}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="linktype">{g->text text="Google Map Link Behavior"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_GMLink,500)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <select id="linktype" name="{g->formVar var="form[linktype]"}">
                  <option value=3 {if $form.linktype eq 3}selected{/if}>{g->text text="Disable the Link"}</option>
                  <option value=0 {if $form.linktype eq 0}selected{/if}>{g->text text="Show the Default Map"}</option>
                  <option value=1 {if $form.linktype eq 1}selected{/if}>{g->text text="Dynamic Link"}</option>
                  <option value=2 {if $form.linktype eq 2}selected{/if}>{g->text text="Show Map filtered on current album"}</option>
                </select>
            </td>
        </tr>
        <tr>
            <th><label for="AdminHelp">{g->text text="Help on the Admin Pages"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_AdminHelp,500)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="checkbox" id="AdminHelp" name="{g->formVar var="form[AdminHelp]"}"
                {if $form.AdminHelp}checked="checked"{/if}
                 value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="UserHelp">{g->text text="Help on the User's Pages"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_UserHelp,500)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="checkbox" id="UserHelp" name="{g->formVar var="form[UserHelp]"}"
                {if $form.UserHelp}checked="checked"{/if}
                 value="1"/>
            </td>
        </tr>
   </table>
</div>

<div class="gbBlock">
<h3> {g->text text="Get via a Map Default Settings"} </h3>
  <p class="giDescription">{g->text text="This section will let you select the default settings for the 'Get via a Map' option"}</p>
    <table class="gbDataTable">
        <tr>
            <th><label for="GVMCenter">{g->text text="Default Map Center"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_DefCenter,640)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="text" id="GVMCenter" name="{g->formVar var="form[GVMCenter]"}" value="{$form.GVMCenter}" maxlength="30" size="30"/>
                {g->text text="Format: latitude,longitude"}
                <input type="submit" value="{g->text text="Get via a Map"}" name="{g->formVar var="form[action][getviamap2]"}"/>
                {if isset($form.error.GVMCenter)}
                    <span class="giError">
                        {g->text text="Please make sure you have formatted the coordinates correctly"}
                    </span>
                {/if}
            </td>
        </tr>
        <tr>
            <th><label for="GVMZoom">{g->text text="Default Map Zoom"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_DefZoom,640)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="text" id="GVMZoom" name="{g->formVar var="form[GVMZoom]"}" value="{$form.GVMZoom}" maxlength="2" size="3"/>
                {if isset($form.error.GVMZoom)}
                    <span class="giError">
                        {g->text text="Please specify an integer between 0 and 17"}
                    </span>
                {/if}
            </td>
        </tr>
    </table>
</div>

<div class="gbBlock">
<h3> {g->text text="Choose feature(s) to enable"} </h3>
  <p class="giDescription">
    {g->text text="This section will let you enable or disable the feature depending on what you need"}
  </p>
    <table class="gbDataTable">
        <tr>
            <th><label for="GoogleOverviewFeature">{g->text text="Enable the Google Overview Feature"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_F_GoogleOverview,850)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="checkbox" id="GoogleOverviewFeature"
                    name="{g->formVar var="form[GoogleOverviewFeature]"}"
                    {if $form.GoogleOverviewFeature}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="GZoomFeature">{g->text text="Enable the GZoom Feature"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_F_GZoom,850)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="checkbox" id="GZoomFeature"
                    name="{g->formVar var="form[GZoomFeature]"}"
                    {if $form.GZoomFeature}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="TFeature">{g->text text="Enable the Theme Feature"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_F_Theme,850)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="checkbox" id="TFeature"
                    name="{g->formVar var="form[ThemeFeature]"}"
                    {if $form.ThemeFeature}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="MFeature">{g->text text="Enable the MarkerSets Feature"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_F_Marker,850)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="checkbox" id="MFeature"
                    name="{g->formVar var="form[MarkerFeature]"}"
                    {if $form.MarkerFeature}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="LFeature">{g->text text="Enable the Legend Feature"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_F_Legend,850)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="checkbox" id="LFeature"
                    name="{g->formVar var="form[LegendFeature]"}"
                    {if $form.LegendFeature}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="FFeature">{g->text text="Enable the Filter Feature"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_F_Filter,850)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="checkbox" id="FFeature"
                    name="{g->formVar var="form[FilterFeature]"}"
                    {if $form.FilterFeature}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="GFeature">{g->text text="Enable the Group Feature"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_F_Group,850)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="checkbox" id="GFeature"
                    name="{g->formVar var="form[GroupFeature]"}"
                    {if $form.GroupFeature}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="RFeature">{g->text text="Enable the Route Feature"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_F_Routes,850)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="checkbox" id="RFeature"
                    name="{g->formVar var="form[RouteFeature]"}"
                    {if $form.RouteFeature}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
    </table>
</div>
<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][save]"}"  value="{g->text text="Save Settings"}"/>
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>

{else} {* Map profile key add/edit *}

<!--
Add a Map Key section
-->
<div class="gbBlock">
   {if isset($form.error.profileDeleted)}
   <h2 class="giError">{g->text text="Profile was deleted in another session."}</h2>
   {/if}
   {if isset($form.error.profileModified)}
   <h2 class="giError">{g->text text="Profile was modified in another session."}</h2>
   {/if}
    {if !empty($form.editProfile)}
        <!-- Return the original data to the controller to detect simultaneous modification -->
	<input type="hidden" name="{g->formVar var="form[oldProfileName]"}" value="{$form.editProfile.name}"/>
	<input type="hidden" name="{g->formVar var="form[oldProfileUrl]"}" value="{$form.editProfile.url}"/>
	<input type="hidden" name="{g->formVar var="form[oldProfileApiKey]"}" value="{$form.editProfile.apiKey}"/>
	<h3>{g->text text="Edit a Google API Key"}</h3>
    {else}
    <h3>{g->text text="Add a Google API Key"}</h3>
    {/if}
  <p class="giDescription">
    {g->text text="This section will add a key to be used with certain settings of the webserver. If you are unsure of the values, leave the defaults."}
  </p>
      <table class="gbDataTable">
        <tr>
            <th><label for="profileName">{g->text text="Profile Name"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_KP_Name,190)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="text" id="profileName"  name="{g->formVar var="form[editProfile][name]"}"
                {if isset($form.editProfile.name)}value="{$form.editProfile.name}"
                {elseif isset($form.baseUrl)}value="{$form.baseUrl}"{/if}/>
                {if isset($form.error.editProfile.name.missing)}<span class="giError">{g->text text="Enter a profile name"}</span>{/if}
            </td>
        </tr>
        <tr>
            <th><label for="profileUrl">{g->text text="Server URL (e.g. http://www.myserver.com)"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_KP_Server,190)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="text" id="profileUrl"  name="{g->formVar var="form[editProfile][url]"}"
                {if isset($form.editProfile.url)}value="{$form.editProfile.url}"
                {elseif isset($form.baseUrl)}value="{$form.baseUrl}"{/if}/>
                {if isset($form.error.editProfile.url.missing)}<span class="giError">{g->text text="Enter a Server URL"}</span>{/if}
            </td>
        </tr>
        <tr>
            <th><label for="profileApiKey">{g->text text="Google API Key"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_KP_GKey,190)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input type="text" id="profileApiKey" style="font-size: smaller;"
                    name="{g->formVar var="form[editProfile][apiKey]"}"
                    maxlength="100" size="60"
                {if isset($form.editProfile.apiKey)}value="{$form.editProfile.apiKey}"{/if}/>
                {if isset($form.error.editProfile.apiKey.missing)}<span class="giError">{g->text text="Enter a Google API Key"}</span> {/if}
                {if isset($form.editProfile.url)}
                    {assign var="requestUrl" value="`$form.editProfile.url`"}
                {elseif isset($form.baseUrl)}
                    {assign var="requestUrl" value="`$form.baseUrl`"}
		{/if}
		{if isset($requestUrl)}
                  {g->text text="Click %shere%s to get the key for" hint="Get the Google API key"
                    arg1="<a target=\"_new\" href=\"http://www.google.com/maps/api_signup?url=`$requestUrl`\">"
		    arg2="</a>"} {$requestUrl}
		{/if}
            </td>
        </tr>
      </table>
</div>
<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][saveProfile]"}" value="{g->text text="Save Settings"}"/>
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>
{/if}