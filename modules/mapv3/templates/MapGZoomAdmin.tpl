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
<!--
End of error/success displaying
-->

{include file="modules/mapv3/templates/MapAdminTab.tpl" mode="GZoom"}

<!-- Help Div -->
<div id="helpdiv" style="visibility:hidden;border:2px solid black;position:absolute;right:350px;top:200px;width:250px;height:150px;">
  <div id="helphead" style="width:250px;height:25px;background:#150095;color:white;border-bottom:2px solid black;"><img alt="none" {if !$form.IE}style="float:left;" src="{$form.picbase}help.png"{else}src='{$form.picbase}blank.gif' style='float:left;filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");'{/if}/><table style="height:100%;border-collapse:collapse;"><tr><td style="width:230px;vertical-align:middle;"><b>Help Window</b></td><td style="width:25px;cursor:pointer;" onclick="javascript:hidehelp()"><center><b>X</b></center></td></tr></table></div>
  <div id ="helptext" style="width:250px;height:123px;color:black;font-size:12px;background:#B2E4F6;-moz-opacity:.75;filter:alpha(opacity=75);opacity:.75;overflow:auto;clear:both;">
  </div>
</div>
<!-- End of Help Div -->

<div class="gbBlock">
  <h3> {g->text text="GZoom Extension"} </h3>

  <p class="giDescription">
    {g->text text="This section allows you to manage the GZoom Extension"}
  </p>
      <table class="gbDataTable">
        <tr>
            <th><label for="GZoom">{g->text text="Ena<u>b</u>le"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_GZ_Enable,400)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <input accesskey="b" type="checkbox" id="GZoom"
                    name="{g->formVar var="form[GZoom]"}"
                    {if $form.GZoom}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="GZPos">{g->text text="Window Position"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_GZ_Position,400)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}
            <td>
                <select id="GZPos" name="{g->formVar var="form[GZPos]"}">
                <option value ="0" {if $form.GZPos eq "0"}selected{/if}/>{g->text text="Top Left Corner"}</option>
                <option value ="2" {if $form.GZPos eq "2"}selected{/if}/>{g->text text="Bottom Left Corner"}</option>
                <option value ="1" {if $form.GZPos eq "1"}selected{/if}/>{g->text text="Top Right Corner"}</option>
                <option value ="3" {if $form.GZPos eq "3"}selected{/if}/>{g->text text="Bottom Right Corner"}</option>
                </select>
                {g->text text="Offset (left-right)"}<input type="text" size="3" name="{g->formVar var="form[GZPosOffX]"}" value="{$form.GZPosOffX}"/>
                {g->text text="Offset (top-bottom)"}<input type="text" size="3" name="{g->formVar var="form[GZPosOffY]"}" value="{$form.GZPosOffY}"/>
            </td>
        </tr>
      </table>
</div>
<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Settings"}"/>
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>