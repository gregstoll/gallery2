{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <p class="giDescription">
    {g->text text="In order for Gallery to properly display this animation, it has to know the animation's width and height.  If you have installed a graphics toolkit that can handle animation types, then these values may already be set.  If not, they may be set to 0.  If you do not enter the correct values here, the animation may appear distorted when viewed in your web browser.  Note that changing these values does <b>not</b> change the animation itself so experimenting won't cause harm."}
  </p>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Width"}
    </td><td>
      <input type="text" size="5" name="{g->formVar var="form[width]"}" value="{$form.width}"/>
    </td>
  </tr>
  {if isset($form.error.width.invalid)}
  <tr>
    <td colspan="2"i class="giError">
      {g->text text="You must enter a value greater than or equal to 0"}
    </td>
  </tr>
  {/if}

  <tr>
    <td>
      {g->text text="Height"}
    </td><td>
      <input type="text" size="5" name="{g->formVar var="form[height]"}" value="{$form.height}"/>
    </td>
  </tr>
  {if isset($form.error.height.invalid)}
  <tr>
    <td colspan="2" class="giError">
      {g->text text="You must enter a value greater than or equal to 0"}
    </td>
  </tr>
  {/if}
  </table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
</div>
