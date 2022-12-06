{*
 * $Revision: 1264 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  {if !empty($form.fields)}
  {if !empty($form.apiKey)}
      <script src="//maps.googleapis.com/maps/api/js?file=api&amp;v=3&amp;key={$form.apiKey}"
              type="text/javascript"></script>
  {/if}
  <script type="text/javascript">
  // <![CDATA[
  {if !empty($form.apiKey)}
  var getAddress = null;
  (function(){ldelim}
      var gps_id = '{g->formVar var="form[fields][GPS]"}';
      var loading_id = 'geocode_loading';
      var loading_msg = '{g->text text="Loading..." forJavascript=true}';
      var msg_error = '{g->text text="Not found" hint="Address not found" forJavascript=true}';
      {literal}
      var geocoder = new google.maps.Geocoder();

      function _getAddress(address) {
          if (address != '') {
              GPSField = document.getElementsByName(gps_id)[0];
              loading_element = document.getElementById(loading_id);
              loading_element.innerHTML = loading_msg;
              geocoder.geocode({'address': address}, function (results, status) {
                  if (status === 'OK') {
                      var location = results[0].geometry.location
                      GPSField.value = location.toUrlValue(6);
                      GPSField.focus();
                  } else {
                      GPSField.value = msg_error;
                      console.debug('Geocode was not successful for the following reason: ' + status);
                  }
                  loading_element.innerHTML = "";
              });
          }
      }

      getAddress = _getAddress;
      {/literal}
  {rdelim})();
  {/if}

  {literal}
  function hidehelp(){
     var helpdiv = document.getElementById('helpdiv');
     helpdiv.style.visibility = "hidden";
  }

  function showhelp(help,pos){
    var helptext = document.getElementById('helptext');
    var helpdiv = document.getElementById('helpdiv');
    helptext.innerHTML = help;
    helpdiv.style.top = pos+"px";
    helpdiv.style.visibility = "visible";
  }
  {/literal}
  {include file="modules/mapv3/templates/helpfile.tpl"}
  // ]]>
  </script>
  
  <!-- Help Div -->
  <div id="helpdiv" style="visibility:hidden;border:2px solid black;position:absolute;right:350px;top:200px;width:250px;height:150px;">
    <div id="helphead" style="width:250px;height:25px;background:#150095;color:white;border-bottom:2px solid black;"><img alt="none" {if !$form.IE}style="float:left;" src="{$form.picbase}help.png"{else}src='{$form.picbase}blank.gif' style='float:left;filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");'{/if}/><table style="height:100%;border-collapse:collapse;"><tr><td style="width:230px;vertical-align:middle;"><b>Help Window</b></td><td style="width:25px;cursor:pointer;" onclick="javascript:hidehelp()"><center><b>X</b></center></td></tr></table></div>
	    <div id="helptext" style="width:250px;height:123px;color:black;font-size:12px;background:#B2E4F6;-moz-opacity:.75;filter:alpha(opacity=75);opacity:.75;overflow:auto;clear:both;">
    </div>
  </div>
  <!-- End of Help Div -->
  
  <table class="gbDataTable">
  {foreach from=$form.fields key=field item=value}
    <tr><th>{$field}</th>
    {if isset($form.UserHelp) and $form.UserHelp eq 1}
	<th>
		<img onclick="javascript:showhelp(_HP_U_{$field},180)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/>
	</th>
	{/if}
	<td>
    {if isset($form.choices[$field])}
      <select name="{g->formVar var="form[fields][$field]"}">
	<option value="">&laquo; {g->text text="No Value"} &raquo;</option>
	{foreach from=$form.choices[$field] item=choice}
	<option{if $choice==$value} selected="selected"{/if}>{$choice}</option>{/foreach}
      </select>
    {else}
      <input type="text" size="40" name="{g->formVar var="form[fields][$field]"}" value="{$value}"/>
       {if ($field eq 'GPS')}
       {if isset($form.UserHelp) and $form.UserHelp eq 1}
		<img onclick="javascript:showhelp(_HP_U_GetViaMap,180)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/>
   	   {/if}
        <a accesskey="g" style="border-top:2px solid #cecece; border-left:2px solid #cecece; border-bottom:2px solid #4a4a4a;border-right:2px solid #4a4a4a;padding:.2em;padding-right:1em;padding-left:1em;text-decoration:none;background-color:#ebebeb;color:#000;font-weight:normal;font-size:12px;" href="{g->url arg1="view=mapv3.ShowMap" arg2="itemId=`$form.itemId`" arg3="plugin=ItemEdit" arg4="Mode=Pick"}">{g->text text="<u>G</u>et via a Map"}</a>
        {if !isset($form.noexif) or (isset($form.noexif) and $form.noexif neq 1)}
          {if isset($form.exif)}
       {if isset($form.UserHelp) and $form.UserHelp eq 1}
		<img onclick="javascript:showhelp(_HP_U_GetViaExif,180)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/>
   	   {/if}
            <input type="submit" class="inputTypeSubmit" name="{g->formVar var="form[action][getexif]"}" value="{g->text text="Get via EXIF headers"}"/>
            <input type="hidden" name="{g->formVar var="form[exif]"}" value="{$form.exif}"/>
          {/if}
        {/if}
       {if $ItemAdmin.item.entityType eq "GalleryPhotoItem"}
       {if isset($form.UserHelp) and $form.UserHelp eq 1}
		<img onclick="javascript:showhelp(_HP_U_WriteToExif,180)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/>
   	   {/if}
        <input type="submit" class="inputTypeSubmit" name="{g->formVar var="form[action][setexif]"}" value="{g->text text="Write GPS to EXIF header"}"/>
        <br>
          {if isset($form.error.gps.missingGPSCoordinates)}
          <div class="giError">
            {g->text text="No GPS coordinates to write."}
          </div>
          {/if}
          {if isset($form.error.gps.coordinatesAlreadyInHeader)}
          <div class="giError">
            {g->text text="This picture already has GPS coordinates in the header"}
          </div>
          {/if}
        {/if}
	{if !empty($form.apiKey)}
        <br>
        <input accesskey="e" type="text" name="{g->formVar var="form[address]"}" id="map.addr" size=40/> <i> {g->text text="<u>E</u>nter Address and "}-&gt; </i>
       {if isset($form.UserHelp) and $form.UserHelp eq 1}
		<img onclick="javascript:showhelp(_HP_U_GetViaAddress,180)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/>
   	   {/if}
        <input type="button" class="inputTypeSubmit" onclick="getAddress(document.getElementById('map.addr').value)" value="{g->text text="Get via address"}"/>
        <span id="geocode_loading"></span>
        {/if}
       {/if}
    {/if}
    </td></tr>
  {/foreach}
  </table>
  {/if}
</div>

{if !empty($form.fields)}
<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit" name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit" name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
{/if}
