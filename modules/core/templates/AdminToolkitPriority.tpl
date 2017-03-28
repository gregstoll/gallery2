{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<script type="text/javascript">{literal}
function swap(j,k) {
 var tt = document.getElementById('t.'+k).innerHTML,
     ti = document.getElementById('i.'+k).value;
 document.getElementById('t.'+k).innerHTML = document.getElementById('t.'+j).innerHTML;
 document.getElementById('i.'+k).value = document.getElementById('i.'+j).value;
 document.getElementById('t.'+j).innerHTML = tt;
 document.getElementById('i.'+j).value = ti;
}
function up(j) { swap(j,j-1); }
function down(j) { swap(j,j+1); }
{/literal}</script>

<div class="gbBlock gcBackground1">
  <h2> {g->text text="Toolkit Priority"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="The toolkits shown below have some overlap in supported operations and mime types.  Rank the toolkits here to determine which will perform these operations."}
  </p>
  <table class="gbDataTable"><tr>
    <th> {g->text text="Toolkit"} </th>
    <th colspan="2"> {g->text text="Order"} </th>
  </tr>
  {foreach from=$AdminToolkitPriority.list key=toolkit item=priority}
    {counter assign=idx}
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td id="t.{$idx}">
	{$toolkit}
      </td><td>
	{if $idx>1}
	  <a href="" onclick="up({$idx});this.blur();return false" style="padding:0 2px">
	    {g->text text="up"}
	  </a>
	{/if}
      </td><td>
	{if $idx<$AdminToolkitPriority.count}
	  <a href="" onclick="down({$idx});this.blur();return false" style="padding:0 2px">
	    {g->text text="down"}
	  </a>
	{/if}
	<input type="hidden" id="i.{$idx}"
	 name="{g->formVar var="form[priority][$priority]"}" value="{$toolkit}"/>
      </td>
    </tr>
  {/foreach}
  </table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
