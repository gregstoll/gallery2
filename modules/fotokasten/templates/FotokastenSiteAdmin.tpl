{*
 * $Revision: 17413 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Fotokasten Settings"} </h2>
</div>

{if isset($status.saved) || isset($status.usingGalleryId)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.saved)}
    {g->text text="Settings saved successfully"}
  {elseif isset($status.usingGalleryId)}
    {g->text text="Using Gallery Affiliate Id"}
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Enter affiliate settings."}
  </p>
  <table class="gbDataTable">
    <tr><td>
      <label for="affiliateId">
	{g->text text="Affiliate Id:"}
      </label>
    </td><td>
      <input type="text" id="affiliateId" size="6"
       name="{g->formVar var="form[affiliateId]"}" value="{$form.affiliateId}"/>

      {if isset($form.error.affiliateId)}
	<span class="giError"> {g->text text="Enter a valid affiliate id"} </span>
      {/if}
    </td></tr><tr><td>
      <label for="affiliateIdPass">
	{g->text text="Affiliate Password:"}
      </label>
    </td><td>
      <input type="text" id="affiliateIdPass" size="34"
       name="{g->formVar var="form[affiliateIdPass]"}" value="{$form.affiliateIdPass}"/>

      {if isset($form.error.affiliateIdPass)}
	<span class="giError"> {g->text text="Enter a valid affiliate password"} </span>
      {/if}
    </td></tr>
  </table>
</div>

<div class="gbBlock">
  {if $FotokastenSiteAdmin.usingDefaultGalleryAffiliateId}
  {g->text text="You're currently using the default Gallery affiliate id.  This means that the Gallery project will receive a small commission for each print made from your website."}
  {else}
  {g->text text="You have specified your own affiliate id."}
  {/if}
  {g->text text="For more information on Fotokasten affiliates, please contact Fotokasten directly via their website at %s." arg1="<a href=\"http://www.fotokasten.de\">http://www.fotokasten.de</a>"}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
  {if !$FotokastenSiteAdmin.usingDefaultGalleryAffiliateId}
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][useGalleryId]"}" value="{g->text text="Use Gallery Affiliate Id"}"
  {/if}
</div>
