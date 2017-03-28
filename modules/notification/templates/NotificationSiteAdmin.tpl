{*
 * $Revision: 17802 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}

<div class="gbBlock gcBackground1">
<h2>{g->text text="Site Notification Administration"}</h2>
</div>

{if isset($status.saved)}
<div class="gbBlock">
<h2 class="giSuccess">{g->text text="Settings saved successfully"}</h2>
</div>
{/if}

<script type="text/javascript">
  // <![CDATA[
  {literal}
  function enableInput(iteration) {
    var eventValue = eval('(' + document.getElementById('select' + iteration).value + ')');
    var disabled = '' == eventValue.name;
    document.getElementById('handler' + iteration).disabled = disabled;
    document.getElementById('enable' + iteration).disabled = disabled;
    document.getElementById('selectValue' + iteration).value = eventValue.name;

    if (eventValue.isGlobal) {
	document.getElementById('public' + iteration).style.visibility = 'hidden';
    } else {
	document.getElementById('public' + iteration).style.visibility = 'visible';
    }
    if (!disabled) {
      var nextRow = document.getElementById('row' + (iteration + 1));
      if (nextRow != null) {
	nextRow.style.display = 'block';
      }
    }
  }

  function enablePublic(iteration) {
    var checked = document.getElementById('enable' + iteration).checked;
    var disabled = !checked;
    document.getElementById('public' + iteration).disabled = disabled;
  }
  {/literal}
  // ]]>
</script>
<div class="gbBlock gcBackground1">
  <div class="yui-g" style="width:800px">
    <div class="yui-g first" style="width:315px">
      <div class="yui-u first" style="width:150px">{g->text text="Description"}</div>
      <div class="yui-u" style="width:150px">{g->text text="Handler"}</div>
    </div>
    <div class="yui-gd" style="width:485px">
      <div class="yui-u first" style="width:170px">
	  <div class="yui-u first" style="width:80px; text-align:center">{g->text text="Enabled"}</div>
	  <div class="yui-u" style="width:80px; text-align:center">{g->text text="Public"}</div>
      </div>
      <div class="yui-u" style="width:295px">&nbsp;</div>
    </div>
  </div>
</div>
<div class="gbBlock">
<input type="hidden" name="{g->formVar var="form[totalRows]"}" value="{$form.totalRows}" />
<input type="hidden" name="{g->formVar var="form[displayRows]"}" value="{$form.displayRows}" />
{assign var="eventCount" value=$form.notificationMap|@count}
{section name="events" loop=$form.totalRows}
  {assign var=iteration value=$smarty.section.events.iteration}
  <div class="yui-g" id="row{$iteration}" style="width:800px{if $iteration>$form.displayRows}; display:none{/if}">
    <div class="yui-g first" style="width:315px">
      <div class="yui-u first" style="width:150px">
	<input type="hidden" name="{g->formVar var="form[notificationMap][$iteration][currentName]"}"
		{if $iteration <= $eventCount}value="{$form.notificationMap[$iteration].currentName}"{/if} />
	<input type="hidden" name="{g->formVar var="form[notificationMap][$iteration][notificationName]"}"
		id="selectValue{$iteration}" {if $iteration <= $eventCount}value="{$form.notificationMap[$iteration].currentName}"{/if}/>
	<select id="select{$iteration}" onchange="enableInput({$iteration})" style="width:100%">
	  {foreach from=$definedEvents item=eventDescription key=notificationName}
	    {if !empty($eventDescription.global)}
	      {assign var=isGlobal value='true'}
	    {else}
	      {assign var=isGlobal value='false'}
	    {/if}
	    <option value="{ldelim}'name': '{$notificationName}', 'isGlobal' : {$isGlobal}{rdelim}"
	      {if !empty($form.notificationMap[$iteration].notificationName) && $notificationName==$form.notificationMap[$iteration].notificationName} selected="selected"{/if}>
	      {$eventDescription.description}
	    </option>
	  {/foreach}
	</select>
      </div>
      <div class="yui-u" style="width:150px">
	<input type="hidden" name="{g->formVar var="form[notificationMap][$iteration][currentHandler]"}"
		{if $iteration <= $eventCount}value="{$form.notificationMap[$iteration].currentHandler}"{/if} />
	<select id="handler{$iteration}" name="{g->formVar var="form[notificationMap][$iteration][handler]"}"
		style="width:100%" {if $iteration >= $form.displayRows} disabled="disabled"{/if}>
	  {foreach from=$eventHandlers item=handlerDescription key=handlerName}
	    <option value="{$handlerName}" {if !empty($form.notificationMap[$iteration].handler) && $handlerName==$form.notificationMap[$iteration].handler}selected="selected"{/if}>{$handlerDescription}</option>
	  {/foreach}
	</select>
      </div>
    </div>
    <div class="yui-g" style="width:485px">
      <div class="yui-u first" style="width:170px">
	<div class="yui-u first" style="width:80px; text-align:center">
	  <input type="checkbox" id="enable{$iteration}" name="{g->formVar var="form[notificationMap][$iteration][enabled]"}"
		onclick="enablePublic({$iteration})"
		{if $iteration >= $form.displayRows} disabled="disabled"{/if}
		{if !empty($form.notificationMap[$iteration].enabled)} checked="checked"{/if}/>
	</div>
	<div class="yui-u" style="width:80px; text-align:center">
	  <input type="checkbox" id="public{$iteration}" name="{g->formVar var="form[notificationMap][$iteration][public]"}"
		{if !empty($form.notificationMap[$iteration].isGlobal)}style="visibility: hidden"{/if}
		{if $iteration >= $form.displayRows || empty($form.notificationMap[$iteration].enabled)} disabled="disabled"{/if}
		{if !empty($form.notificationMap[$iteration].public)}checked="checked"{/if}/>
	  </div>
      </div>
      {if $iteration < count($form.notificationMap) && isset($form.error.notificationMap[$iteration].noEventHandler)}
      <div class="yui-u giError" style="width:295px">
	<span>
	    {g->text text="Event Handler is required"}
	</span>
      </div>
      {/if}
    </div>
  </div>
{/section}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit" name="{g->formVar var="form[action][save]"}"
	 value="{g->text text="Save"}" />
  <input type="submit" class="inputTypeSubmit" name="{g->formVar var="form[action][reset]"}"
	 value="{g->text text="Reset"}"/>
</div>
