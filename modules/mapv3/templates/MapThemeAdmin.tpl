{*
 * $Revision: 1264 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}

<div class="gbBlock gcBackground1">
    <h2>{g->text text="Map Module Settings"}   <a style="position:relative;left:50px;border-top:2px solid #cecece; border-left:2px solid #cecece; border-bottom:2px solid #4a4a4a;border-right:2px solid #4a4a4a;padding:.2em;padding-right:1em;padding-left:1em;text-decoration:none;background-color:#ebebeb;color:#000;font-weight:normal;font-size:12px;" href="{g->url arg1="view=mapv3.ShowMap"}">{g->text text="Show Google Map"}</a></h2>
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
<!--
End of error/success displaying
-->

{include file="modules/mapv3/templates/MapAdminTab.tpl" mode="Theme"}

<!-- Help Div -->
<div id="helpdiv" style="visibility:hidden;border:2px solid black;position:absolute;right:350px;top:200px;width:250px;height:150px;">
    <div id="helphead" style="width:250px;height:25px;background:#150095;color:white;border-bottom:2px solid black;"><img alt="none" {if !$form.IE}style="float:left;" src="{$form.picbase}help.png"{else}src='{$form.picbase}blank.gif' style='float:left;filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");'{/if}/><table style="height:100%;border-collapse:collapse;"><tr><td style="width:230px;vertical-align:middle;"><b>Help Window</b></td><td style="width:25px;cursor:pointer;" onclick="javascript:hidehelp()"><center><b>X</b></center></td></tr></table></div>
    <div id="helptext" style="width:250px;height:123px;color:black;font-size:12px;background:#B2E4F6;-moz-opacity:.75;filter:alpha(opacity=75);opacity:.75;overflow:auto;clear:both;">
    </div>
</div>
<!-- End of Help Div -->

<!--
Theme settings
-->
<div class="gbBlock">
  <h3> {g->text text="Control Settings"}</h3>

  <p class="giDescription">{g->text text="This section allows you to manage Map Control settings"}</p>
      <table class="gbDataTable">
        <tr>
            <th><label for="MapControlSize">{g->text text="Map Control Size / Type"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_T_ControlType,200)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <select name="{g->formVar var="form[MapControlType]"}" value="{$form.MapControlType}">
                <option value="Large"{if $form.MapControlType eq "Large"} selected{/if}>
                Stock - Large</option>
                <option value="Small"{if $form.MapControlType eq "Small"} selected{/if}>
                Stock - Small</option>
                <option value="None"{if $form.MapControlType eq "None"} selected{/if}>
                None</option>
                {$form.controllist}
                </select>
                {g->text text="Control the position below."}
            </td>
        </tr>
        <tr>
            <th><label for="MapControlPos">{g->text text="Control position"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_T_ControlPosition,200)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <select id="MapControlPos" name="{g->formVar var="form[MapControlPos]"}">
                <option value ="0" {if $form.MapControlPos eq "0"}selected{/if}/>{g->text text="Top Left Corner"}</option>
                <option value ="2" {if $form.MapControlPos eq "2"}selected{/if}/>{g->text text="Bottom Left Corner"}</option>
                <option value ="1" {if $form.MapControlPos eq "1"}selected{/if}/>{g->text text="Top Right Corner"}</option>
                <option value ="3" {if $form.MapControlPos eq "3"}selected{/if}/>{g->text text="Bottom Right Corner"}</option>
                </select>
                {g->text text="Offset (left-right)"}<input id="MapControlPosOffX" type="text" size="3" maxlength="3" name="{g->formVar var="form[MapControlPosOffX]"}" value="{$form.MapControlPosOffX}"/>
                {g->text text="Offset (top-bottom)"}<input id="MapControlPosOffY" type="text" size="3" maxlength="3" name="{g->formVar var="form[MapControlPosOffY]"}" value="{$form.MapControlPosOffY}"/>
                {if isset($form.error.MapControlPosOffX) || isset($form.error.MapControlPosOffX) }
                    <span class="giError">
                        {g->text text="Please make sure you have formatted the offset(s) correctly."}
                    </span>
                {/if}
            </td>
        </tr>
        <tr>
            <th><label for="ShowMapType">{g->text text="Show Map Ty<u>p</u>e"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_T_ControlMapType,200)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <input accesskey="p" type="checkbox" id="ShowMapType"
                    name="{g->formVar var="form[showMapType]"}"
                    {if $form.showMapType}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
   </table>
</div>

<div class="gbBlock">
  <h3> {g->text text="InfoWindow Settings"}</h3>

  <p class="giDescription">{g->text text="This section allows you to manage InfoWindow settings"}</p>
      <table class="gbDataTable">
        <tr>
            <th><label for="MapWindowType">{g->text text="Map InfoWindow Template"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_T_InfoWindowTemplate,400)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <select name="{g->formVar var="form[MapWindowType]"}" value="{$form.MapWindowType}">
                {$form.windowlist}
                </select>
            </td>
        </tr>
        <tr>
            <th><label for="ZoomInLevel">{g->text text="Default Zoom <u>I</u>n Level"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_T_DefaultZoomIn,400)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <input accesskey="i" type="text" id="ZoomInLevel"
                    name="{g->formVar var="form[zoomInLevel]"}"
                    value="{$form.zoomInLevel}" maxlength="2" size="2"/>
                {g->text text="The default zoom level when a 'zoom in' link is clicked. Specify an integer from 0 to 17."}
                {if isset($form.error.zoomInLevel)}
                    <span class="giError">
                        {g->text text="Please specify an integer between 0 to 17"}
                    </span>
                {/if}
            </td>
        </tr>
        <tr>
            <th><label for="ShowZoomLinks">{g->text text="Show \"Zoom in\" Li<u>n</u>ks"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_T_ZoomInLinks,400)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <input accesskey="n" type="checkbox" id="ShowZoomLinks"
                    name="{g->formVar var="form[showZoomLinks]"}"
                    {if $form.showZoomLinks}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="ShowItemSummaries">{g->text text="Show Item S<u>u</u>mmaries"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_T_ItemSummaries,400)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <input accesskey="u" type="checkbox" id="ShowItemSummaries"
                    name="{g->formVar var="form[showItemSummaries]"}"
                    {if $form.showItemSummaries}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="ShowItemDescriptions">{g->text text="Show Item <u>D</u>escriptions"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_T_ItemDescription,400)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <input accesskey="d" type="checkbox" id="ShowItemDescriptions"
                    name="{g->formVar var="form[showItemDescriptions]"}"
                    {if $form.showItemDescriptions}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
  </table>
</div>
<div class="gbBlock">
  <h3> {g->text text="Thumbnail bar settings"}</h3>

  <p class="giDescription">
    {g->text text="This section manages the thumbnail bar visibility and basic settings"}
  </p>
      <table class="gbDataTable">
        <tr>
            <th><label for="ThumbBarPos">{g->text text="Thumbnail Bar Position"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_T_ThumbnailBarPos,550)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <select name="{g->formVar var="form[ThumbBarPos]"}" value="{$form.ThumbBarPos}">
                  <option value="hidden">{g->text text="Hide" hint="Don't display"}</option>
                  <option value="top" {if $form.ThumbBarPos eq "top"}selected{/if}>{g->text text="Top" hint="Opposite of bottom"}</option>
                  <option value="bottom" {if $form.ThumbBarPos eq "bottom"}selected{/if}>{g->text text="Bottom" hint="Opposite of top"}</option>
                  <option value="right" {if $form.ThumbBarPos eq "right"}selected{/if}>{g->text text="Right" hint="Opposite of left"}</option>
                  {*<option value="left" {if $form.ThumbBarPos eq "left"}selected{/if}>{g->text text="Left" hint="Opposite of right"}</option> Ain't working right*}
                </select>
            </td>
        </tr>
        <tr>
            <th><label for="ThumbHeight">{g->text text="Thumbnail Size"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_T_ThumbnailBarSize,550)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <input type="text" id="ThumbHeight" name="{g->formVar var="form[ThumbHeight]"}" value="{$form.ThumbHeight}" maxlength="5" size="5"/>
                {g->text text="Height for TOP &amp; BOTTOM position, Width for LEFT &amp; RIGHT position"}
                {if isset($form.error.ThumbHeight)}
                    <span class="giError">
                        {g->text text="Please specify a positive integer."}
                    </span>
                {/if}
            </td>
        </tr>
  </table>
</div>

<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Settings"}"/>
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>
