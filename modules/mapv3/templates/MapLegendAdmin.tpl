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

{include file="modules/mapv3/templates/MapAdminTab.tpl" mode="Legend"}

<!-- Help Div -->
<div id="helpdiv" style="visibility:hidden;border:2px solid black;position:absolute;right:350px;top:200px;width:250px;height:150px;">
    <div id="helphead" style="width:250px;height:25px;background:#150095;color:white;border-bottom:2px solid black;"><img alt="none" {if !$form.IE}style="float:left;" src="{$form.picbase}help.png"{else}src='{$form.picbase}blank.gif' style='float:left;filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");'{/if}/><table style="height:100%;border-collapse:collapse;"><tr><td style="width:230px;vertical-align:middle;"><b>Help Window</b></td><td style="width:25px;cursor:pointer;" onclick="javascript:hidehelp()"><center><b>X</b></center></td></tr></table></div>
    <div id="helptext" style="width:250px;height:123px;color:black;font-size:12px;background:#B2E4F6;-moz-opacity:.75;filter:alpha(opacity=75);opacity:.75;overflow:auto;clear:both;">
    </div>
</div>
<!-- End of Help Div -->

<!--
Displaying the Legend management area
-->
<div class="gbBlock">
  <h3> {g->text text="Legend Management"} </h3>

  <p class="giDescription">
    {g->text text="This section allows you to create and manage a legend"}
  </p>
   <table class="gbDataTable">
   <tr>
     <th colspan=3><label for="PhotoLegend">{g->text text="Main Options"}:</label></th>
   </tr>
   <tr>
     <th><label for="LegendPos">{g->text text="Legend Position"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_L_LegendPos,220)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <select name="{g->formVar var="form[LegendPos]"}">
          <option value="right">{g->text text="Right" hint="Opposite of left"}</option>
          <option value="left" {if isset($form.LegendPos) and $form.LegendPos eq "left"}selected{/if}>{g->text text="Left" hint="Opposite of right"}</option>
          <option value="top" {if isset($form.LegendPos) and $form.LegendPos eq "top"}selected{/if}>{g->text text="Top" hint="Opposite of bottom"}</option>
          <option value="bottom" {if isset($form.LegendPos) and $form.LegendPos eq "bottom"}selected{/if}>{g->text text="Bottom" hint="Opposite of top"}</option>
          <option value="hide" {if isset($form.LegendPos) and $form.LegendPos eq "hide"}selected{/if}>{g->text text="Hide" hint="Don't display"}</option>
          </select>
        </td>
   </tr>
   <tr>
     <th colspan=3><label for="PhotoLegend">{g->text text="Photo Legend"}:</label></th>
   </tr>
   <tr>
     <th><label for="UsePhotoLegend">{g->text text="Activate <u>P</u>hoto Legend"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_L_LegendPhoto,300)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input accesskey="p" type="checkbox" name ="usephotolegend" value="1"{if $form.PhotoLegend}checked{/if}/>
        </td>
   </tr>
   <tr>
     <th><label for="ExpandPhotoLegend">{g->text text="Initially Expand Legend"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_L_LegendPhotoExp,300)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input type="checkbox" name ="ExpandPhotoLegend" value="1"{if $form.ExpandPhotoLegend}checked{/if}/>
          {g->text text="Not valid when legend is located on the top or bottom"}
        </td>
   </tr>
   {foreach from=$form.allmarkers key=num item=imagelist}
   {foreach from=$imagelist key=name item=images}
     {if $name eq $form.useMarkerSet}
     {foreach from=$images item=image key=num}
       <tr {if !$form.PhotoLegend}style="display:none;"{/if}>
         <th>
           {$image}
         </th>
         <td colspan=2>
           <input type="text" size=20 name="P{$num}" {if $form.PhotoLegends neq ""}value="{$form.PhotoLegends[$num]}"{/if}></input>
         </td>
       </tr>
       {/foreach}
     {/if}
   {/foreach}
   {/foreach}
   <tr>
     <th colspan=3><label for="AlbumLegend">{g->text text="Album Legend"}:</label></th>
   </tr>
   <tr>
     <th><label for="UseAlbumLegend">{g->text text="Activate Album <u>L</u>egend"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_L_LegendAlbum,350)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input accesskey="l" type="checkbox" name ="usealbumlegend" value="1" {if $form.AlbumLegend}checked{/if}/>
        </td>
   </tr>
   <tr>
     <th><label for="ExpandAlbumLegend">{g->text text="Initially Expand Legend"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_L_LegendAlbumExp,350)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input type="checkbox" name ="ExpandAlbumLegend" value="1"{if $form.ExpandAlbumLegend}checked{/if}/>
          {g->text text="Not valid when legend is located on the top or bottom"}
        </td>
   </tr>
   {foreach from=$form.allmarkers key=num item=imagelist}
   {foreach from=$imagelist key=name item=images}
     {if $name eq $form.useAlbumMarkerSet}
     {foreach from=$images item=image key=num}
       <tr {if !$form.AlbumLegend}style="display:none;"{/if}>
         <th>
           {$image}
         </th>
         <td colspan=2>
           <input type="text" size=20 name="A{$num}" {if $form.AlbumLegends neq ""}value="{$form.AlbumLegends[$num]}"{/if}></input>
         </td>
       </tr>
       {/foreach}
     {/if}
   {/foreach}
   {/foreach}
   <tr>
     <th colspan=3><label for="GroupLegend">{g->text text="Group Legend"}:</label></th>
   </tr>
   <tr>
     <th><label for="UseGroupLegend">{g->text text="Activate Group Legend"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_L_LegendGroup,450)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input type="checkbox" name ="UseGroupLegend" value="1"{if $form.GroupLegend}checked{/if}/>
        </td>
   </tr>
   <tr>
     <th><label for="ExpandGroupLegend">{g->text text="Initially Expand Legend"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_L_LegendGroupExp,450)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input type="checkbox" name ="ExpandGroupLegend" value="1"{if $form.ExpandGroupLegend}checked{/if}/>
          {g->text text="Not valid when legend is located on the top or bottom"}
        </td>
   </tr>
   {foreach from=$form.allmarkers key=num item=imagelist}
   {foreach from=$imagelist key=name item=images}
     {if $name eq $form.useGroupMarkerSet}
     {foreach from=$images item=image key=num}
       <tr {if !$form.GroupLegend}style="display:none;"{/if}>
         <th>
           {$image}
         </th>
         <td colspan=2>
           <input type="text" size=20 name="G{$num}" {if $form.GroupLegends neq ""}value="{$form.GroupLegends[$num]}"{/if}></input>
         </td>
       </tr>
       {/foreach}
     {/if}
   {/foreach}
   {/foreach}
  </table>
<div>

<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Legend"}"/>
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>