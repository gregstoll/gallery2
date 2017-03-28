{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
	<h4> {g->text text="Rating"} </h4>
	<p class="giDescription">
	{g->text text="This will enable or disable ratings for this album and, optionally, for its subalbums.  You can use permissions to allow viewing or adding ratings for specific users or groups."}
	</p>
	<input type="checkbox" id="rating.enabled" name="{g->formVar var="form[rating][enabled]"}"
   	{if $form.rating.enabled} checked="checked"{/if}/> {g->text text="Enable rating for this album"}
	<br/>
    {g->changeInDescendents module="rating" text="... and for all subalbums"}
</div>
