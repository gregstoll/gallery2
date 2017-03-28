{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<form id="SearchScan" action="{g->url}" method="post">
  <div id="gsContent" class="gcBorder1">
    <div class="gbBlock gcBackground1">
      <h2> {g->text text="Search the Gallery"} </h2>
    </div>

    {g->hiddenFormVars}
    <input type="hidden" name="{g->formVar var="controller"}" value="{$SearchScan.controller}"/>
    <input type="hidden" name="{g->formVar var="form[formName]"}" value="SearchScan"/>

    <script type="text/javascript">
      // <![CDATA[
      function setCheck(val) {ldelim}
	{foreach from=$SearchScan.modules key=moduleId item=moduleInfo}
	  {foreach from=$moduleInfo.options key=optionId item=optionInfo}
	    document.getElementById('cb_{$moduleId}_{$optionId}').checked = val;
	  {/foreach}
	{/foreach}
      {rdelim}

      function invertCheck() {ldelim}
	var o;
	{foreach from=$SearchScan.modules key=moduleId item=moduleInfo}
	  {foreach from=$moduleInfo.options key=optionId item=optionInfo}
	    o = document.getElementById('cb_{$moduleId}_{$optionId}'); o.checked = !o.checked;
	  {/foreach}
	{/foreach}
      {rdelim}
      // ]]>
    </script>

    <div class="gbBlock">
      <input type="text" size="50"
       name="{g->formVar var="form[searchCriteria]"}" value="{$form.searchCriteria}"/>
      <script type="text/javascript">
        document.getElementById('SearchScan')['{g->formVar var="form[searchCriteria]"}'].focus();
      </script>
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][search]"}" value="{g->text text="Search"}"/>

      {if isset($form.error.searchCriteria.missing)}
      <div class="giError">
	{g->text text="You must enter some text to search for!"}
      </div>
      {/if}

      <div style="margin: 0.5em 0">
	{foreach from=$SearchScan.modules key=moduleId item=moduleInfo}
	  {foreach from=$moduleInfo.options key=optionId item=optionInfo}
	  <input type="checkbox" id="cb_{$moduleId}_{$optionId}"
	   name="{g->formVar var="form[options][$moduleId][$optionId]"}"
	   {if isset($form.options.$moduleId.$optionId)} checked="checked"{/if}/>
	  <label for="cb_{$moduleId}_{$optionId}">
	    {$optionInfo.description}
	  </label>
	  {/foreach}
	{/foreach}
      </div>

      <div>
	<a href="javascript:setCheck(1)">{g->text text="Check All"}</a>
	&nbsp;
	<a href="javascript:setCheck(0)">{g->text text="Uncheck All"}</a>
	&nbsp;
	<a href="javascript:invertCheck()">{g->text text="Invert"}</a>
      </div>
    </div>

    {assign var="resultCount" value="0"}
    {if !empty($SearchScan.searchResults)}
    {foreach from=$SearchScan.searchResults key=moduleId item=results}
      {assign var="resultCount" value=$resultCount+$results.count}

      <div class="gbBlock">
	<h4>
	  {$SearchScan.modules.$moduleId.name}
	  {if ($results.count > 0)}
	    {g->text text="Results %d - %d" arg1=$results.start arg2=$results.end}
	  {/if}
	  {if ($results.count > $results.end)}
	    {assign var="moduleId" value=$moduleId}
	    &nbsp;
	    <input type="submit" class="inputTypeSubmit"
	     name="{g->formVar var="form[action][showAll][$moduleId]"}"
	     value="{g->text text="Show all %d" arg1=$results.count}"/>
	  {/if}
	</h4>

	{assign var="searchCriteria" value=$form.searchCriteria}
	{if (sizeof($results.results) > 0)}
	  <table><tr>
	    {foreach from=$results.results item=result}
	      {assign var=itemId value=$result.itemId}
	      <td class="{if
		$SearchScan.items.$itemId.canContainChildren}gbItemAlbum{else}gbItemImage{/if}"
		style="width: 10%">
		<a href="{g->url arg1="view=core.ShowItem" arg2="itemId=$itemId"}">
		{if isset($SearchScan.thumbnails.$itemId)}
		  {g->image item=$SearchScan.items.$itemId image=$SearchScan.thumbnails.$itemId
			    class="giThumbnail"}
		{else}
		{g->text text="No thumbnail"}
		{/if}
		</a>
		<ul class="giInfo">
		  {foreach from=$result.fields item=field}
		    {if isset($field.value)}
		    <li>
		      <span class="ResultKey">{$field.key}:</span>
		      <span class="ResultData">{$field.value|default:"&nbsp;"|markup}</span>
		    </li>
		    {/if}
		  {/foreach}
		</ul>
	      </td>
	    {/foreach}
	  </tr></table>
	  <script type="text/javascript">
	    search_HighlightResults('{$searchCriteria}');
	  </script>
	{else}
	  <p class="giDescription">
	    {g->text text="No results found for"} '{$form.searchCriteria}'
	  </p>
	{/if}
      </div>
    {/foreach}
    {/if}

    {if $resultCount>0 && $SearchScan.slideshowAvailable}
    <div class="gbBlock gcBackground1">
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][slideshow]"}"
       value="{g->text text="View these results in a slideshow"}"/>
    </div>
    {/if}
  </div>
</form>
