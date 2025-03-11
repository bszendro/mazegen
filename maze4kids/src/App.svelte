<script lang="ts">
  import logoIcon from './assets/logo44.svg'
  import { Navbar, NavBrand } from 'flowbite-svelte';
  import DesignForm from './DesignForm.svelte';
  import type { BuildMazeParams } from './lib/maze/PaperSize';
  import PreviewPage from './PreviewPage.svelte';

  // Hand-made routing just for the sake of learning how it works
  const defaultNavigationState = {
    page: 'main',
  };
  let navigationState = $state(history.state ?? defaultNavigationState);

  window.addEventListener('popstate', (e) => {
    navigationState = e.state ?? defaultNavigationState;
  });

  function navigateTo(page: string, params = {}) {
    const newState = { page, params };
    history.pushState(newState, '');
    navigationState = newState;
  }

  function handleBuild(params: BuildMazeParams) {
    navigateTo('preview', params);
  }
</script>

<main>
  <Navbar>
    <NavBrand>
      <img src="{logoIcon}" class="me-3" alt="Logo" />
      <span class="self-center whitespace-nowrap text-xl font-semibold dark:text-white">Maze4Kids</span>
    </NavBrand>
  </Navbar>

  {#if navigationState.page === 'main'}
    <DesignForm onBuild={handleBuild} />
  {:else if navigationState.page === 'preview'}
    <PreviewPage buildParams={navigationState.params} />
  {:else}
    <div>Unknown navigation state</div>
  {/if}

</main>
