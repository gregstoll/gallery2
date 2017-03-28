{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Album Select Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Albums can be displayed in a simple list box or a dynamic tree."}
  </p>

  <h4> {g->text text="Sort order"} </h4>

  <input type="radio" id="rbSortManual" {if $form.sort=='manual'}checked="checked" {/if}
   name="{g->formVar var="form[sort]"}" value="manual"/>
  <label for="rbSortManual">
    {g->text text="Use manual sort order of albums"}
  </label>
  <br/>

  <input type="radio" id="rbSortTitle" {if $form.sort=='title'}checked="checked" {/if}
   name="{g->formVar var="form[sort]"}" value="title"/>
  <label for="rbSortTitle">
    {g->text text="Sort albums by title"}
  </label>
  <br/>

  <input type="radio" id="rbSortAlbum" {if $form.sort=='album'}checked="checked" {/if}
   name="{g->formVar var="form[sort]"}" value="album"/>
  <label for="rbSortAlbum">
    {g->text text="Use sort order defined in each album (affects performance)"}
  </label>

  <h4> {g->text text="Tree"} </h4>

  <input type="checkbox" id="cbLines" {if $form.treeLines}checked="checked" {/if}
   name="{g->formVar var="form[treeLines]"}"/>
  <label for="cbLines">
    {g->text text="Connect tree branches with lines"}
  </label>
  <br/>

  <input type="checkbox" id="cbIcons" {if $form.treeIcons}checked="checked" {/if}
   name="{g->formVar var="form[treeIcons]"}"/>
  <label for="cbIcons">
    {g->text text="Show folder icons"}
  </label>
  <br/>

  <input type="checkbox" id="cbCookies" {if $form.treeCookies}checked="checked" {/if}
   name="{g->formVar var="form[treeCookies]"}"/>
  <label for="cbCookies">
    {g->text text="Use cookies to remember which branches are open"}
  </label>
  <br/>

  <input type="checkbox" id="cbExpandCollapse" {if $form.treeExpandCollapse}checked="checked" {/if}
   name="{g->formVar var="form[treeExpandCollapse]"}"/>
  <label for="cbExpandCollapse">
    {g->text text="Show expand-all and collapse-all options"}
  </label>
  <br/>

  <input type="checkbox" id="cbCloseSameLevel" {if $form.treeCloseSameLevel}checked="checked" {/if}
   name="{g->formVar var="form[treeCloseSameLevel]"}"/>
  <label for="cbCloseSameLevel">
    {g->text text="Only one branch within a parent can be expanded at the same time. Expand/collapse functions are disabled with this option."}
  </label>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
