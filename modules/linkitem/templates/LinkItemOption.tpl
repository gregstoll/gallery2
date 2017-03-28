{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <h3> {g->text text="Link"} </h3>

  <h4> {g->text text="URL:"} </h4>
  <input type="text" size="60"
   name="{g->formVar var="form[LinkItemOption][link]"}" value="{$form.LinkItemOption.link}"/>

  {if isset($form.LinkItemOption.error.link.missing)}
  <div class="giError">
    {g->text text="Missing URL"}
  </div>
  {/if}
</div>
