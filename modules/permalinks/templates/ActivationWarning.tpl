{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}

<div class="gbBlock">
  <h3> {g->text text="Permalink post activation"} </h3>

{capture assign="link"}
  <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=rewrite.AdminRewrite"}">
    {g->text text="URL Rewrite Module"}</a>
{/capture}

  <p>{g->text text="Now that you have activated the Permalinks module, you have to activate Permalinks rule in the %s." arg1="`$link`"} </p>
</div>
