{*
 * $Revision: 1264 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}

<div class="gbBlock gcBackground1">
    <h2>{g->text text="Map Module Settings"}<a style="position:relative;left:50px;border-top:2px solid #cecece; border-left:2px solid #cecece; border-bottom:2px solid #4a4a4a;border-right:2px solid #4a4a4a;padding:.2em;padding-right:1em;padding-left:1em;text-decoration:none;background-color:#ebebeb;color:#000;font-weight:normal;font-size:12px;" href="{g->url arg1="view=mapv3.ShowMap"}">{g->text text="Show Google Map"}</a></h2>
</div>

<!--
Beginning of error/success displaying
-->
{if isset($status.saved)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="Settings have been saved!"}</h2>
    </div>
{/if}
{if isset($status.changesucess)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="Template file successfully updated, backup file created"}</h2>
    </div>
{/if}
{if !empty($form.error)}
<div class="gbBlock">
   <h2 class="giError">{g->text text="There was a problem processing your request."}</h2>
</div>
{/if}
{if isset($form.error.canotopentemplate)}
    <div class="gbBlock">
        <h2 class="giError">{g->text text="Error opening file, check permissions on the DefaultTheme/template folder"}</h2>
    </div>
{/if}
{if isset($status.routedeleted)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="Route successfully deleted"}</h2>
    </div>
{/if}
{if isset($status.routesaved)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="Route successfully edited and saved"}</h2>
    </div>
{/if}
<!--
End of error/success displaying
-->

{include file="modules/mapv3/templates/MapAdminTab.tpl" mode="Routes"}

<!-- Help Div -->
<div id="helpdiv" style="visibility:hidden;border:2px solid black;position:absolute;right:350px;top:200px;width:250px;height:150px;">
    <div id="helphead" style="width:250px;height:25px;background:#150095;color:white;border-bottom:2px solid black;"><img alt="none" {if !$form.IE}style="float:left;" src="{$form.picbase}help.png"{else}src='{$form.picbase}blank.gif' style='float:left;filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");'{/if}/><table style="height:100%;border-collapse:collapse;"><tr><td style="width:230px;vertical-align:middle;"><b>Help Window</b></td><td style="width:25px;cursor:pointer;" onclick="javascript:hidehelp()"><center><b>X</b></center></td></tr></table></div>
    <div id="helptext" style="width:250px;height:123px;color:black;font-size:12px;background:#B2E4F6;-moz-opacity:.75;filter:alpha(opacity=75);opacity:.75;overflow:auto;clear:both;">
    </div>
</div>
<!-- End of Help Div -->

<!--
Route Management
-->
{if $form.mode neq 'AddRoute'}
<div class="gbBlock">
  <h3> {g->text text="Route Management"}</h3>
  <div class="gbBlock">
  {if isset($form.badhtmltag)}
    {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
       <img onclick="javascript:showhelp(_HP_R_MainHelp,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/>
    {/if}            
    <span class="giWarning">{g->text text="Your Theme.tpl file does not contain the right HTML tag, routes will not display in Internet Explorer"}
     {g->text text="Refer to %sthe Map Module Wiki%s (Route in IE) for a workaround" arg1="<a href='http://codex.gallery2.org/index.php/Gallery2:Modules:Map#Workaround'>" arg2="</a>"}
     <input type="submit" name="{g->formVar var="form[action][tplupdate]"}" value="{g->text text="Update it for me" hint="Update the Theme.tpl file"}" />
     </span>
  {/if}
  <br>
  <table class="gbDataTable">
     <tr>
       <th><label for="EnableRouteNumber">{g->text text="Enable Route Numbering"}:</label></th>
       {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
           <th><img onclick="javascript:showhelp(_HP_R_ActivateNumbers,180)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
       {/if}
       <td>
          <input type="checkbox" id="EnableRouteNumber"
                 name="{g->formVar var="form[EnableRouteNumber]"}"
                 {if $form.EnableRouteNumber}checked="checked"{/if}
                 value="1"/>
       </td>
     </tr>
  </table>
  </div>
<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][savesettings]"}" value="{g->text text="Save Settings"}"/>
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>

  
  <p class="giDescription">
    {g->text text="This section permits creation of routes and management of the path to be displayed on the map"}
  </p>
  {if isset($form.theroutes) and $form.theroutes != ""}
    <br/>
    <table class="gbDataTable">
      <tr><th>{g->text text="Route ID"}</th><th>{g->text text="Route Name"}</th><th>{g->text text="Route Line Color"}</th><th>{g->text text="Route Line Width"}</th><th>{g->text text="Route Line Opacity"}</th><th>{g->text text="Enabled"}</th><th>{g->text text="Filter"}</th><th colspan=2>{g->text text="Actions"}</th></tr>
    {foreach from=$form.theroutes item=routes}
      <tr style="background-color:{$routes[2]}"><td><font color="black">{$routes[0]}</font></td><td><font color="black">{$routes[1]}</font></td><td><font color="black">{$routes[2]}</font></td><td><font color="black">{$routes[3]}</font></td><td><font color="black">{$routes[4]}</font></td><td><font color="black">{$routes[5]}</font></td><td><font color="black">{$routes[6]}</font></td>
      <td><input type="submit" name="{g->formVar var="form[action][deleteroute]"}" value="{g->text text="Delete Route"} {$routes[0]}" /></td>
      <td><input type="submit" name="{g->formVar var="form[action][editroute]"}" value="{g->text text="Edit Route"} {$routes[0]}" /></td>
      </tr>
    {/foreach}
    </table>
    <br/>
    <input type="submit" name="{g->formVar var="form[action][createroute]"}" value="{g->text text="Add a Route"}"/>
    <input type="submit" name="{g->formVar var="form[action][deleteallroutes]"}" value="{g->text text="Delete All Routes"}"/>
  {else}
    {g->text text="There are no Routes created, click"} <input type="submit" name="{g->formVar var="form[action][createroute]"}" value="{g->text text="Add" hint="Create new"}" />
  {/if}
<div>

{else}

<div class="gbBlock">
  <h3> {g->text text="Route Settings"}</h3>
  <p class="giDescription">
    {g->text text="Settings specific to this route"}
  </p>
  {if isset($form.OldRoute)}
  <input type="hidden" name="edit" value="{$form.OldRoute}"/>
  <input type="hidden" name="oldid" value="{$form.EditRoute.0}"/>
  {/if}
   <table class="gbDataTable">
   <tr>
     <th><label for="RouteName">{g->text text="Enter Route <u>N</u>ame"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_R_Name,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
        <input accesskey="n" type="text" name="RouteName"   {if isset($form.OldRoute)}value="{$form.EditRoute.1}"{/if}/> *
        {if isset($form.error.routename)}
        <span class="giError">
        {g->text text="Please enter a route name."}
        </span>
        {/if}
        </td>
   </tr>
   <tr>
     <th><label for="RouteColor">{g->text text="Input Route Co<u>l</u>or"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_R_Color,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
        {* @todo: Look at using Gallery's ColorChooser.js *}
        <!-- flooble.com Color Picker start -->
        <a accesskey="l" href="Javascript:pickColor('pick1131606682');" id="pick1131606682" class="colorpickinput">{g->text text="Select"}</a>
        <input id="pick1131606682field" size="7" onChange="relateColor('pick1131606682', this.value);" name="RouteColor"  {if isset($form.OldRoute)}value="{$form.EditRoute.2}"{/if}/>
        <script language="javascript">relateColor('pick1131606682', getObj('pick1131606682field').value);</script>
        <noscript><a href="http://www.flooble.com/scripts/colorpicker.php">javascript color picker by flooble</a> | <a href="http://www.flooble.com/scripts/">get free javascript games and effects</a></noscript>
        <!-- flooble Color Picker end -->
        {if isset($form.error.routecolor)}
        <span class="giError">
        {g->text text="Please enter a route color (Format: #RRGGBB) - No color will default to purple"}
        </span>
        {/if}
        </td>
   </tr>
   <tr>
     <th><label for="RouteWeight">{g->text text="Enter Route size in <u>p</u>ixels"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_R_Size,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input accesskey="p" type="text" name="RouteWeight" {if isset($form.OldRoute)}value="{$form.EditRoute.3}"{/if}/>
        </td>
   </tr>
   <tr>
     <th><label for="RouteOpacity">{g->text text="Enter Route <u>O</u>pacity"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_R_Opacity,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input accesskey="o" type="text" name="RouteOpacity" {if isset($form.OldRoute)}value="{$form.EditRoute.4}"{/if}/>
        {if isset($form.error.routeopacity)}
        <span class="giError">
        {g->text text="Please enter the opacity between 0 and 1 - default is 1"}
        </span>
        {/if}
        </td>
   </tr>
   <tr>
     <th><label for="RouteEnabled">{g->text text="Route Enabled?"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_R_Enabled,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <select name="RouteEnabled">
          <option {if isset($form.OldRoute) and $form.EditRoute.5 eq "Yes"}selected{/if} value="Yes">{g->text text="Yes"}</option>
          <option {if isset($form.OldRoute) and $form.EditRoute.5 eq "No"}selected{/if} value="No">{g->text text="No"}</option>
          </select>
        </td>
   </tr>
   <tr>
     <th><label for="RouteFilter">{g->text text="Create a Filter for this route?"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_R_Filter,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <select name="RouteFilter">
          <option {if isset($form.OldRoute) and $form.EditRoute.6 eq "Yes"}selected{/if} value="Yes">{g->text text="Yes"}</option>
          <option {if isset($form.OldRoute) and $form.EditRoute.6 eq "No"}selected{/if} value="No">{g->text text="No"}</option>
          </select>
        </td>
   </tr>
  </table>
</div>

<div class="gbBlock">
  <h3>
    {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
       <img onclick="javascript:showhelp(_HP_R_items,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/>
    {/if}            
    {g->text text="Items in Route"}</h3> {if isset($form.error.toofewarg)}<span class="giError">{g->text text="Please select at least 2 markers."}</span>{/if}
  <p class="giDescription">
    {g->text text="Select the items you want added to this route and the order to add them in"}
  </p>
  {if isset($form.items)}
  <table class="gbDataTable itemtable" style="width:70%">
  <tr><th>{g->text text="Thumbnail"}</th><th>{g->text text="Item Information"}</th><th>{g->text text="Selected"}</th><th>{g->text text="Order"}</th></tr>
  {foreach from=$form.items item=point}
  <tr bgcolor="{if $point.type eq 'GoogleMapGroup'}lightblue{/if}{if $point.type eq 'GalleryPhotoItem'}pink{/if}{if $point.type eq 'GalleryAlbumItem'}lightgreen{/if}"><td><font color="black"><img src="{$point.thumb}" alt="{$point.title}"/></font></td><td><b><font color="black">{g->text text="Title:"}</font></b> <font color="black">{$point.title}</font><br><b><font color="black">{g->text text="Date:"}</font></b> <font color="black">{$point.date}</font><br><i><font color="black">{$point.description}</font></i></td>
  <td><input name="{$point.id}" type="checkbox" value="{$point.id}"
  {foreach from=$form.EditRoute item=route}
   {if $route eq $point.id}checked{/if}
  {/foreach}
  /></td><td><input type="text" name="Order{$point.id}" size="3" {if isset($form.routevalues[$point.id])}value="{$form.routevalues[$point.id]}"{/if}/></td>
  </tr>
  {/foreach}
  </table>
  {else}
  {g->text text="There are no items with GPS coordinates, please add at least two entries to make a route"}
  {/if}
</div>

<div class="gbBlock gcBackground1">
{if isset($form.items)}<input type="submit" name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Route"}"/>{/if}
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>
{/if}