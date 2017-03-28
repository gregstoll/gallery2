{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<script type="text/javascript">{literal}
// <![CDATA[
function move(toRight) {
  var from = document.getElementById(toRight ? 'available' : 'selected'),
        to = document.getElementById(toRight ? 'selected' : 'available'), i;
  for (i = 0; i < from.length; i++) {
    if (from.options[i].selected) {
      to.options[to.length] = new Option(from.options[i].text, from.options[i].value);
      from.options[i--] = null;
    }
  }
}
function selectAll() {
  var s = document.getElementById('selected'), i;
  for (i = 0; i < s.length; i++) {
    s.options[i].selected = true;
  }
}
// ]]>
{/literal}</script>

<div class="gbBlock gcBackground1">
  <h2> {g->text text="MultiLanguage Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Maintain primary language captions using the normal Gallery interface.  Here select the additional languages to support in the MultiLanguage tab."}
  </p>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Available"} </th>
    <th></th>
    <th> {g->text text="Selected"} </th>
  </tr><tr>
    <td>
      <select id="available" size="20" multiple="multiple">
	{html_options options=$MultiLangSiteAdmin.availableList}
      </select>
    </td><td>
      <p>
	<input type="submit" class="inputTypeSubmit"
	 value="&gt;&gt;" onclick="move(1);return false"/>
      </p>
      <p>
	<input type="submit" class="inputTypeSubmit"
	 value="&lt;&lt;" onclick="move();return false"/>
      </p>
    </td><td>
      <select id="selected" size="20" multiple="multiple"
       name="{g->formVar var="form[languages][]"}">
	{html_options options=$MultiLangSiteAdmin.selectedList}
      </select>
    </td>
  </tr></table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit" onclick="selectAll()"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
